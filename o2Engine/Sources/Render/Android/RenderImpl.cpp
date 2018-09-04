#include "stdafx.h"

#ifdef PLATFORM_ANDROID

#include "Render/Render.h"

#include "Application/Application.h"
#include "Assets/Assets.h"
#include "Render/Font.h"
#include "Render/Mesh.h"
#include "Render/Sprite.h"
#include "Render/Texture.h"
#include "Utils/Debug/Debug.h"
#include "Utils/Debug/Log/LogStream.h"
#include "Utils/Math/Geometry.h"
#include "Utils/Math/Interpolation.h"
#include "Application/Input.h"

namespace o2
{
	Render::Render() :
		mReady(false), mStencilDrawing(false), mStencilTest(false), mClippingEverything(false)
	{
		mVertexBufferSize = USHRT_MAX;
		mIndexBufferSize = USHRT_MAX;

		mLog = mnew LogStream("Render");
		o2Debug.GetLog()->BindStream(mLog);

		mResolution = o2Application.GetContentSize();

		CheckCompatibles();
		
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glLineWidth(1.0f);

		glGenBuffers(1, &mVertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, mVertexBufferSize * sizeof(Vertex2), &mVertexData, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2), &((Vertex2*)0)->x);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2), &((Vertex2*)0)->color);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2), &((Vertex2*)0)->tu);

		glGenBuffers(1, &mIndexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferSize * sizeof(unsigned short), &mVertexIndexData, GL_DYNAMIC_DRAW);

		//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

		InitializeStdShader();

		GL_CHECK_ERROR(mLog);

		mLog->Out("GL_VENDOR: " + (String)(char*)glGetString(GL_VENDOR));
		mLog->Out("GL_RENDERER: " + (String)(char*)glGetString(GL_RENDERER));
		mLog->Out("GL_VERSION: " + (String)(char*)glGetString(GL_VERSION));

// 		HDC dc = GetDC(0);
// 		mDPI.x = GetDeviceCaps(dc, LOGPIXELSX);
// 		mDPI.y = GetDeviceCaps(dc, LOGPIXELSY);
// 		ReleaseDC(0, dc);

		InitializeFreeType();
		InitializeLinesIndexBuffer();
		InitializeLinesTextures();

		mCurrentRenderTarget = TextureRef();

		if (IsDevMode())
			o2Assets.onAssetsRebuilded += Func(this, &Render::OnAssetsRebuilded);

		mReady = true;
	}

	Render::~Render()
	{
		if (!mReady)
			return;

		if (IsDevMode())
			o2Assets.onAssetsRebuilded -= Func(this, &Render::OnAssetsRebuilded);

		mSolidLineTexture = TextureRef::Null();
		mDashLineTexture = TextureRef::Null();

		DeinitializeFreeType();

		mReady = false;
	}

	GLuint RenderBase::BuildShaderProgram(const char* vertexSource, const char* fragmentSource)
	{
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		GLint Result = GL_FALSE;
		int InfoLogLength;

		glShaderSource(VertexShaderID, 1, &vertexSource, NULL);
		glCompileShader(VertexShaderID);

		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			fprintf(stdout, "%s \n", &VertexShaderErrorMessage[0]);
		}

		glShaderSource(FragmentShaderID, 1, &fragmentSource, NULL);
		glCompileShader(FragmentShaderID);

		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			fprintf(stdout, "%s \n", &FragmentShaderErrorMessage[0]);
		}

		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ProgramErrorMessage(InfoLogLength+1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			fprintf(stdout, "%s \n", &ProgramErrorMessage[0]);
		}

		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);

		return ProgramID;
	}

	void RenderBase::InitializeStdShader()
	{
		const char* fragShader = "#version 330 core                \n \
        out vec4 color;                                            \n \
                                                                   \n \
        in vec4 fragmentColor;                                     \n \
        in vec2 UV;                                                \n \
                                                                   \n \
        uniform sampler2D textureSampler;                          \n \
                                                                   \n \
        void main()                                                \n \
        {                                                          \n \
            color = fragmentColor*texture( textureSampler, UV );   \n \
        }";

		const char* vtxShader = "#version 330 core                 \n \
        uniform mat4 modelViewProjectionMatrix;                    \n \
                                                                   \n \
        layout(location = 0) in vec3 vertexPosition_modelspace;    \n \
        layout(location = 1) in vec4 vertexColor;                  \n \
        layout(location = 2) in vec2 vertexUV;                     \n \
                                                                   \n \
        out vec4 fragmentColor;                                    \n \
        out vec2 UV;                                               \n \
                                                                   \n \
        uniform mat4 MVP;                                          \n \
                                                                   \n \
        void main(){                                               \n \
            gl_Position = MVP * vec4(vertexPosition_modelspace,1); \n \
            fragmentColor = vertexColor;                           \n \
            UV = vertexUV;                                         \n \
        }";

		mStdShader = BuildShaderProgram(vtxShader, fragShader);
		mStdShaderMvpUniform = glGetUniformLocation(mStdShader, "MVP");
		mStdShaderTextureSample = glGetUniformLocation(mStdShader, "textureSampler");

		glUseProgram(mStdShader);
	}

	void Render::CheckCompatibles()
	{
		//get max texture size
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mMaxTextureSize.x);
		mMaxTextureSize.y = mMaxTextureSize.x;
	}

	void Render::Begin()
	{
		if (!mReady)
			return;

		mLastDrawTexture = NULL;
		mLastDrawVertex = 0;
		mLastDrawIdx = 0;
		mTrianglesCount = 0;
		mFrameTrianglesCount = 0;
		mDIPCount = 0;
		mCurrentPrimitiveType = PrimitiveType::Polygon;

		mDrawingDepth = 0.0f;

		mScissorInfos.Clear();
		mStackScissors.Clear();

		mClippingEverything = false;

		SetupViewMatrix(mResolution);
		UpdateCameraTransforms();

		preRender();
		preRender.Clear();
	}

	void Render::DrawPrimitives()
	{
		if (mLastDrawVertex < 1)
			return;

		static const GLenum primitiveType[3]{ GL_TRIANGLES, GL_TRIANGLES, GL_LINES };

		glDrawElements(primitiveType[(int)mCurrentPrimitiveType], mLastDrawIdx, GL_UNSIGNED_SHORT, (void*)0);

		GL_CHECK_ERROR(mLog);

		mFrameTrianglesCount += mTrianglesCount;
		mLastDrawVertex = mTrianglesCount = mLastDrawIdx = 0;

		mDIPCount++;
	}

	void Render::SetupViewMatrix(const Vec2I& viewSize)
	{
		mCurrentResolution = viewSize;
		UpdateCameraTransforms();
	}

	void Render::End()
	{
		if (!mReady)
			return;

		postRender();
		postRender.Clear();

		DrawPrimitives();

		GL_CHECK_ERROR(mLog);

		CheckTexturesUnloading();
		CheckFontsUnloading();
	}

	void Render::Clear(const Color4& color /*= Color4::Blur()*/)
	{
		glClearColor(color.RF(), color.GF(), color.BF(), color.AF());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GL_CHECK_ERROR(mLog);
	}

	void mtxMultiply(float* ret, const float* lhs, const float* rhs)
	{
		// [ 0 4  8 12 ]   [ 0 4  8 12 ]
		// [ 1 5  9 13 ] x [ 1 5  9 13 ]
		// [ 2 6 10 14 ]   [ 2 6 10 14 ]
		// [ 3 7 11 15 ]   [ 3 7 11 15 ]
		ret[0] = lhs[0]*rhs[0] + lhs[4]*rhs[1] + lhs[8]*rhs[2] + lhs[12]*rhs[3];
		ret[1] = lhs[1]*rhs[0] + lhs[5]*rhs[1] + lhs[9]*rhs[2] + lhs[13]*rhs[3];
		ret[2] = lhs[2]*rhs[0] + lhs[6]*rhs[1] + lhs[10]*rhs[2] + lhs[14]*rhs[3];
		ret[3] = lhs[3]*rhs[0] + lhs[7]*rhs[1] + lhs[11]*rhs[2] + lhs[15]*rhs[3];

		ret[4] = lhs[0]*rhs[4] + lhs[4]*rhs[5] + lhs[8]*rhs[6] + lhs[12]*rhs[7];
		ret[5] = lhs[1]*rhs[4] + lhs[5]*rhs[5] + lhs[9]*rhs[6] + lhs[13]*rhs[7];
		ret[6] = lhs[2]*rhs[4] + lhs[6]*rhs[5] + lhs[10]*rhs[6] + lhs[14]*rhs[7];
		ret[7] = lhs[3]*rhs[4] + lhs[7]*rhs[5] + lhs[11]*rhs[6] + lhs[15]*rhs[7];

		ret[8] = lhs[0]*rhs[8] + lhs[4]*rhs[9] + lhs[8]*rhs[10] + lhs[12]*rhs[11];
		ret[9] = lhs[1]*rhs[8] + lhs[5]*rhs[9] + lhs[9]*rhs[10] + lhs[13]*rhs[11];
		ret[10] = lhs[2]*rhs[8] + lhs[6]*rhs[9] + lhs[10]*rhs[10] + lhs[14]*rhs[11];
		ret[11] = lhs[3]*rhs[8] + lhs[7]*rhs[9] + lhs[11]*rhs[10] + lhs[15]*rhs[11];

		ret[12] = lhs[0]*rhs[12] + lhs[4]*rhs[13] + lhs[8]*rhs[14] + lhs[12]*rhs[15];
		ret[13] = lhs[1]*rhs[12] + lhs[5]*rhs[13] + lhs[9]*rhs[14] + lhs[13]*rhs[15];
		ret[14] = lhs[2]*rhs[12] + lhs[6]*rhs[13] + lhs[10]*rhs[14] + lhs[14]*rhs[15];
		ret[15] = lhs[3]*rhs[12] + lhs[7]*rhs[13] + lhs[11]*rhs[14] + lhs[15]*rhs[15];
	}

	void Render::UpdateCameraTransforms()
	{
		DrawPrimitives();

		float projMat[16];
		Math::OrthoProjMatrix(projMat, 0.0f, (float)mCurrentResolution.x, (float)mCurrentResolution.y, 0.0f, 0.0f, 10.0f);
		glViewport(0, 0, mCurrentResolution.x, mCurrentResolution.y);

		float modelMatrix[16] =
		{
			1,                                      0,                                       0, 0,
			0,                                     -1,                                       0, 0,
			0,                                      0,                                       1, 0,
			Math::Round(mCurrentResolution.x*0.5f), Math::Round(mCurrentResolution.y*0.5f), -1, 1
		};

		Basis defaultCameraBasis((Vec2F)mCurrentResolution*-0.5f, Vec2F::Right()*mCurrentResolution.x, Vec2F().Up()*mCurrentResolution.y);
		Basis camTransf = mCamera.GetBasis().Inverted()*defaultCameraBasis;

		float camTransfMatr[16] =
		{
			camTransf.xv.x,     camTransf.xv.y,     0, 0,
			camTransf.yv.x,     camTransf.yv.y,     0, 0,
			0,                  0,                  0, 0,
			camTransf.origin.x, camTransf.origin.y, 0, 1
		};

		float mvp[16];
		float finalCamMtx[16];
		mtxMultiply(finalCamMtx, modelMatrix, camTransfMatr);
		mtxMultiply(mvp, projMat, finalCamMtx);

		glUniformMatrix4fv(mStdShaderMvpUniform, 1, GL_FALSE, mvp);

		GL_CHECK_ERROR(mLog);

	}

	void Render::BeginRenderToStencilBuffer()
	{
		if (mStencilDrawing || mStencilTest)
			return;

		DrawPrimitives();

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0x1, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		GL_CHECK_ERROR(mLog);

		mStencilDrawing = true;
	}

	void Render::EndRenderToStencilBuffer()
	{
		if (!mStencilDrawing)
			return;

		DrawPrimitives();

		glDisable(GL_STENCIL_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		GL_CHECK_ERROR(mLog);

		mStencilDrawing = false;
	}

	void Render::EnableStencilTest()
	{
		if (mStencilTest || mStencilDrawing)
			return;

		DrawPrimitives();

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_EQUAL, 0x1, 0xffffffff);

		GL_CHECK_ERROR(mLog);

		mStencilTest = true;
	}

	void Render::DisableStencilTest()
	{
		if (!mStencilTest)
			return;

		DrawPrimitives();

		glDisable(GL_STENCIL_TEST);

		mStencilTest = false;
	}

	void Render::ClearStencil()
	{
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);

		GL_CHECK_ERROR(mLog);
	}

	void Render::EnableScissorTest(const RectI& rect)
	{
		DrawPrimitives();

		RectI summaryScissorRect = rect;
		if (!mStackScissors.IsEmpty())
		{
			RectI lastSummaryClipRect = mStackScissors.Last().mSummaryScissorRect;
			mClippingEverything = !summaryScissorRect.IsIntersects(lastSummaryClipRect);
			summaryScissorRect = summaryScissorRect.GetIntersection(lastSummaryClipRect);
			mScissorInfos.Last().mEndDepth = mDrawingDepth;
		}
		else
		{
			glEnable(GL_SCISSOR_TEST);
			GL_CHECK_ERROR(mLog);
		}

		mScissorInfos.Add(ScissorInfo(summaryScissorRect, mDrawingDepth));
		mStackScissors.Add(ScissorStackItem(rect, summaryScissorRect));

		glScissor((int)(summaryScissorRect.left + mCurrentResolution.x*0.5f),
			(int)(summaryScissorRect.bottom + mCurrentResolution.y*0.5f),
				  (int)summaryScissorRect.Width(),
				  (int)summaryScissorRect.Height());
	}

	void Render::DisableScissorTest(bool forcible /*= false*/)
	{
		if (mStackScissors.IsEmpty())
		{
			mLog->WarningStr("Can't disable scissor test - no scissor were enabled!");
			return;
		}

		DrawPrimitives();

		if (forcible)
		{
			glDisable(GL_SCISSOR_TEST);
			GL_CHECK_ERROR(mLog);

			while (!mStackScissors.IsEmpty() && !mStackScissors.Last().mRenderTarget)
				mStackScissors.PopBack();

			mScissorInfos.Last().mEndDepth = mDrawingDepth;
		}
		else
		{
			if (mStackScissors.Count() == 1)
			{
				glDisable(GL_SCISSOR_TEST);
				GL_CHECK_ERROR(mLog);
				mStackScissors.PopBack();

				mScissorInfos.Last().mEndDepth = mDrawingDepth;
				mClippingEverything = false;
			}
			else
			{
				mStackScissors.PopBack();
				RectI lastClipRect = mStackScissors.Last().mSummaryScissorRect;
				glScissor((int)(lastClipRect.left + mCurrentResolution.x*0.5f),
					(int)(lastClipRect.bottom + mCurrentResolution.y*0.5f),
						  (int)lastClipRect.Width(),
						  (int)lastClipRect.Height());

				mScissorInfos.Last().mEndDepth = mDrawingDepth;
				mScissorInfos.Add(ScissorInfo(lastClipRect, mDrawingDepth));

				mClippingEverything = lastClipRect == RectI();
			}
		}
	}

	void Render::DrawBuffer(PrimitiveType primitiveType, Vertex2* vertices, UInt verticesCount,
							UInt16* indexes, UInt elementsCount, const TextureRef& texture)
	{
		if (!mReady)
			return;

		mDrawingDepth += 1.0f;

		if (mClippingEverything)
			return;

		UInt indexesCount;
		if (primitiveType == PrimitiveType::Line)
			indexesCount = elementsCount*2;
		else
			indexesCount = elementsCount*3;

		if (mLastDrawTexture != texture.mTexture ||
			mLastDrawVertex + verticesCount >= mVertexBufferSize ||
			mLastDrawIdx + indexesCount >= mIndexBufferSize ||
			mCurrentPrimitiveType != primitiveType)
		{
			DrawPrimitives();

			mLastDrawTexture = texture.mTexture;
			mCurrentPrimitiveType = primitiveType;

			if (mLastDrawTexture)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mLastDrawTexture->mHandle);
				glUniform1i(mStdShaderTextureSample, 0);

				GL_CHECK_ERROR(mLog);
			}
			else
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
				glUniform1i(mStdShaderTextureSample, 0);

				GL_CHECK_ERROR(mLog);
			}
		}

		memcpy(&mVertexData[mLastDrawVertex], vertices, sizeof(Vertex2)*verticesCount);

		for (UInt i = mLastDrawIdx, j = 0; j < indexesCount * 3; i++, j++)
            mVertexIndexData[i] = mLastDrawVertex + indexes[j];

		if (primitiveType != PrimitiveType::Line)
			mTrianglesCount += elementsCount;

		mLastDrawVertex += verticesCount;
		mLastDrawIdx += indexesCount;
	}

	void Render::SetRenderTexture(TextureRef renderTarget)
	{
		if (!renderTarget)
		{
			UnbindRenderTexture();
			return;
		}

		if (renderTarget->mUsage != Texture::Usage::RenderTarget)
		{
			mLog->Error("Can't set texture as render target: not render target texture");
			UnbindRenderTexture();
			return;
		}

		if (!renderTarget->IsReady())
		{
			mLog->Error("Can't set texture as render target: texture isn't ready");
			UnbindRenderTexture();
			return;
		}

		DrawPrimitives();

		if (!mStackScissors.IsEmpty())
		{
			mScissorInfos.Last().mEndDepth = mDrawingDepth;
			glDisable(GL_SCISSOR_TEST);
			GL_CHECK_ERROR(mLog);
		}

		mStackScissors.Add(ScissorStackItem(RectI(), RectI(), true));

		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->mFrameBuffer);
		GL_CHECK_ERROR(mLog);

		SetupViewMatrix(renderTarget->GetSize());

		mCurrentRenderTarget = renderTarget;
	}

	void Render::UnbindRenderTexture()
	{
		if (!mCurrentRenderTarget)
			return;

		DrawPrimitives();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		GL_CHECK_ERROR(mLog);

		SetupViewMatrix(mResolution);

		mCurrentRenderTarget = TextureRef();

		DisableScissorTest(true);
		mStackScissors.PopBack();
		if (!mStackScissors.IsEmpty())
		{
			glEnable(GL_SCISSOR_TEST);
			GL_CHECK_ERROR(mLog);

			auto clipRect = mStackScissors.Last().mSummaryScissorRect;

			glScissor((int)(clipRect.left + mCurrentResolution.x*0.5f),
				(int)(clipRect.bottom + mCurrentResolution.y*0.5f),
					  (int)clipRect.Width(),
					  (int)clipRect.Height());

			mClippingEverything = clipRect == RectI();
		}
	}
}

#endif // PLATFORM_ANDROID