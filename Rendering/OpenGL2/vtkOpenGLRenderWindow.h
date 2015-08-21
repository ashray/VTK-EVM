/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpenGLRenderWindow.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkOpenGLRenderWindow - OpenGL rendering window
// .SECTION Description
// vtkOpenGLRenderWindow is a concrete implementation of the abstract class
// vtkRenderWindow. vtkOpenGLRenderer interfaces to the OpenGL graphics
// library. Application programmers should normally use vtkRenderWindow
// instead of the OpenGL specific version.

#ifndef vtkOpenGLRenderWindow_h
#define vtkOpenGLRenderWindow_h

#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkRenderWindow.h"
#include <map> // for ivar

#include "vtk_glew.h" // Needed for GLuint.

class vtkIdList;
class vtkOpenGLHardwareSupport;
class vtkTextureUnitManager;
class vtkOpenGLShaderCache;
class vtkStdString;
class vtkTexture;
class vtkTextureObject;
class vtkShaderProgram;

namespace vtkgl
{
class VertexArrayObject;
}

class VTKRENDERINGOPENGL2_EXPORT vtkOpenGLRenderWindow : public vtkRenderWindow
{
public:
  vtkTypeMacro(vtkOpenGLRenderWindow, vtkRenderWindow);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Overridden to release resources that would interfere with an external
  // application's rendering.
  void Render();

  // Description:
  // Set/Get the maximum number of multisamples
  static void SetGlobalMaximumNumberOfMultiSamples(int val);
  static int  GetGlobalMaximumNumberOfMultiSamples();

  // Description:
  // Update system if needed due to stereo rendering.
  virtual void StereoUpdate();

  // Description:
  // Set/Get the pixel data of an image, transmitted as RGBRGB...
  virtual unsigned char *GetPixelData(int x,int y,int x2,int y2,int front);
  virtual int GetPixelData(int x,int y,int x2,int y2, int front,
                           vtkUnsignedCharArray *data);
  virtual int SetPixelData(int x,int y,int x2,int y2,unsigned char *data,
                           int front);
  virtual int SetPixelData(int x,int y,int x2,int y2,
                           vtkUnsignedCharArray *data, int front);

  // Description:
  // Set/Get the pixel data of an image, transmitted as RGBARGBA...
  virtual float *GetRGBAPixelData(int x,int y,int x2,int y2,int front);
  virtual int GetRGBAPixelData(int x,int y,int x2,int y2, int front,
                               vtkFloatArray* data);
  virtual int SetRGBAPixelData(int x,int y,int x2,int y2, float *data,
                               int front, int blend=0);
  virtual int SetRGBAPixelData(int x,int y,int x2,int y2, vtkFloatArray *data,
                               int front, int blend=0);
  virtual void ReleaseRGBAPixelData(float *data);
  virtual unsigned char *GetRGBACharPixelData(int x,int y,int x2,int y2,
                                              int front);
  virtual int GetRGBACharPixelData(int x,int y,int x2,int y2, int front,
                                   vtkUnsignedCharArray *data);
  virtual int SetRGBACharPixelData(int x, int y, int x2, int y2,
                                   unsigned char *data, int front,
                                   int blend=0);
  virtual int SetRGBACharPixelData(int x,int y,int x2,int y2,
                                   vtkUnsignedCharArray *data, int front,
                                   int blend=0);

  // Description:
  // Set/Get the zbuffer data from an image
  virtual float *GetZbufferData( int x1, int y1, int x2, int y2 );
  virtual int GetZbufferData( int x1, int y1, int x2, int y2, float* z );
  virtual int GetZbufferData( int x1, int y1, int x2, int y2,
                              vtkFloatArray* z );
  virtual int SetZbufferData( int x1, int y1, int x2, int y2, float *buffer );
  virtual int SetZbufferData( int x1, int y1, int x2, int y2,
                              vtkFloatArray *buffer );


  // Description:
  // Activate a texture unit for this texture
  void ActivateTexture(vtkTextureObject *);

  // Description:
  // Deactive a previously activated texture
  void DeactivateTexture(vtkTextureObject *);

  // Description:
  // Get the texture unit for a given texture object
  int GetTextureUnitForTexture(vtkTextureObject *);

  // Description:
  // Get the size of the depth buffer.
  int GetDepthBufferSize();

  // Description:
  // Get the size of the color buffer.
  // Returns 0 if not able to determine otherwise sets R G B and A into buffer.
  int GetColorBufferSizes(int *rgba);

  // Description:
  // Initialize OpenGL for this window.
  virtual void OpenGLInit();

  // Initialize the state of OpenGL that VTK wants for this window
  virtual void OpenGLInitState();

  // Initialize VTK for rendering in a new OpenGL context
  virtual void OpenGLInitContext();

  // Description::
  // Get if the context includes opengl core profile 3.2 support
  static bool GetContextSupportsOpenGL32();
  void SetContextSupportsOpenGL32(bool val);

  // Description:
  // Return the OpenGL name of the back left buffer.
  // It is GL_BACK_LEFT if GL is bound to the window-system-provided
  // framebuffer. It is vtkgl::COLOR_ATTACHMENT0_EXT if GL is bound to an
  // application-created framebuffer object (GPU-based offscreen rendering)
  // It is used by vtkOpenGLCamera.
  unsigned int GetBackLeftBuffer();

  // Description:
  // Return the OpenGL name of the back right buffer.
  // It is GL_BACK_RIGHT if GL is bound to the window-system-provided
  // framebuffer. It is vtkgl::COLOR_ATTACHMENT0_EXT+1 if GL is bound to an
  // application-created framebuffer object (GPU-based offscreen rendering)
  // It is used by vtkOpenGLCamera.
  unsigned int GetBackRightBuffer();

  // Description:
  // Return the OpenGL name of the front left buffer.
  // It is GL_FRONT_LEFT if GL is bound to the window-system-provided
  // framebuffer. It is vtkgl::COLOR_ATTACHMENT0_EXT if GL is bound to an
  // application-created framebuffer object (GPU-based offscreen rendering)
  // It is used by vtkOpenGLCamera.
  unsigned int GetFrontLeftBuffer();

  // Description:
  // Return the OpenGL name of the front right buffer.
  // It is GL_FRONT_RIGHT if GL is bound to the window-system-provided
  // framebuffer. It is vtkgl::COLOR_ATTACHMENT0_EXT+1 if GL is bound to an
  // application-created framebuffer object (GPU-based offscreen rendering)
  // It is used by vtkOpenGLCamera.
  unsigned int GetFrontRightBuffer();

  // Description:
  // Return the OpenGL name of the back left buffer.
  // It is GL_BACK if GL is bound to the window-system-provided
  // framebuffer. It is vtkgl::COLOR_ATTACHMENT0_EXT if GL is bound to an
  // application-created framebuffer object (GPU-based offscreen rendering)
  // It is used by vtkOpenGLCamera.
  unsigned int GetBackBuffer();

  // Description:
  // Return the OpenGL name of the front left buffer.
  // It is GL_FRONT if GL is bound to the window-system-provided
  // framebuffer. It is vtkgl::COLOR_ATTACHMENT0_EXT if GL is bound to an
  // application-created framebuffer object (GPU-based offscreen rendering)
  // It is used by vtkOpenGLCamera.
  unsigned int GetFrontBuffer();

  // Description:
  // @deprecated Replaced by
  // vtkOpenGLCheckErrorMacro
  VTK_LEGACY(virtual void CheckGraphicError());
  VTK_LEGACY(virtual int HasGraphicError());
  VTK_LEGACY(virtual const char *GetLastGraphicErrorString());

  // Description:
  // Get the time when the OpenGL context was created.
  virtual unsigned long GetContextCreationTime();

  // Description:
  // Returns an Shader Cache object
  vtkGetObjectMacro(ShaderCache,vtkOpenGLShaderCache);

  // Description:
  // Returns its texture unit manager object. A new one will be created if one
  // hasn't already been set up.
  vtkTextureUnitManager *GetTextureUnitManager();

  // Description:
  // Block the thread until the actual rendering is finished().
  // Useful for measurement only.
  virtual void WaitForCompletion();

  // Description:
  // Helper function that draws a quad on the screen
  // at the specified vertex coordinates and if
  // tcoords are not NULL with the specified
  // texture coordinates.
  static void RenderQuad(
    float *verts, float *tcoords,
    vtkShaderProgram *program, vtkgl::VertexArrayObject *vao);
  static void RenderTriangles(
    float *verts, unsigned int numVerts,
    GLuint *indices, unsigned int numIndices,
    float *tcoords,
    vtkShaderProgram *program, vtkgl::VertexArrayObject *vao);

  // Description:
  // Replacement for the old glDrawPixels function
  virtual void DrawPixels(int x1, int y1, int x2, int y2,
              int numComponents, int dataType, void *data);

  // Description:
  // Replacement for the old glDrawPixels function, but it allows
  // for scaling the data and using only part of the texture
  virtual void DrawPixels(
    int dstXmin, int dstYmin, int dstXmax, int dstYmax,
    int srcXmin, int srcYmin, int srcXmax, int srcYmax,
    int srcWidth, int srcHeight, int numComponents, int dataType, void *data);

  // Description:
  // Replacement for the old glDrawPixels function.  This simple version draws all
  // the data to the entire current viewport scaling as needed.
  virtual void DrawPixels(
    int srcWidth, int srcHeight, int numComponents, int dataType, void *data);

protected:
  vtkOpenGLRenderWindow();
  ~vtkOpenGLRenderWindow();

  vtkOpenGLShaderCache *ShaderCache;

  long OldMonitorSetting;

  std::map<const vtkTextureObject *, int> TextureResourceIds;

  int GetPixelData(int x, int y, int x2, int y2, int front, unsigned char* data);
  int GetRGBAPixelData(int x, int y, int x2, int y2, int front, float* data);
  int GetRGBACharPixelData(int x, int y, int x2, int y2, int front,
                           unsigned char* data);

  // Description:
  // Create an offScreen window based on OpenGL framebuffer extension.
  // Return if the creation was successful or not.
  // \pre positive_width: width>0
  // \pre positive_height: height>0
  // \pre not_initialized: !OffScreenUseFrameBuffer
  // \post valid_result: (result==0 || result==1)
  //                     && (result implies OffScreenUseFrameBuffer)
  int CreateHardwareOffScreenWindow(int width, int height);

  // Description:
  // Destroy an offscreen window based on OpenGL framebuffer extension.
  // \pre initialized: OffScreenUseFrameBuffer
  // \post destroyed: !OffScreenUseFrameBuffer
  void DestroyHardwareOffScreenWindow();

  // Description:
  // Flag telling if a framebuffer-based offscreen is currently in use.
  int OffScreenUseFrameBuffer;

  // Description:
  // Variables used by the framebuffer-based offscreen method.
  int NumberOfFrameBuffers;
  unsigned int TextureObjects[4]; // really GLuint
  unsigned int FrameBufferObject; // really GLuint
  unsigned int DepthRenderBufferObject; // really GLuint

  // Description:
  // Create a not-off-screen window.
  virtual void CreateAWindow() = 0;

  // Description:
  // Destroy a not-off-screen window.
  virtual void DestroyWindow() = 0;

  // Description:
  // Free up any graphics resources associated with this window
  virtual void ReleaseGraphicsResources();

  // Description:
  // Set the texture unit manager.
  void SetTextureUnitManager(vtkTextureUnitManager *textureUnitManager);


  // Description:
  // Query and save OpenGL state
  void SaveGLState();

  // Description:
  // Restore OpenGL state at end of the rendering
  void RestoreGLState();

  std::map<std::string, int> GLStateIntegers;

  unsigned int BackLeftBuffer;
  unsigned int BackRightBuffer;
  unsigned int FrontLeftBuffer;
  unsigned int FrontRightBuffer;
  unsigned int FrontBuffer;
  unsigned int BackBuffer;

  #ifndef VTK_LEGACY_REMOVE
  // Description:
  // @deprecated Replaced by
  // vtkOpenGLCheckErrorMacro
  unsigned int LastGraphicError;
  #endif

  // Description:
  // Flag telling if the context has been created here or was inherited.
  int OwnContext;

  vtkTimeStamp ContextCreationTime;

  vtkTextureUnitManager *TextureUnitManager;

  vtkTextureObject *DrawPixelsTextureObject;

  bool Initialized; // ensure glewinit has been called

private:
  vtkOpenGLRenderWindow(const vtkOpenGLRenderWindow&);  // Not implemented.
  void operator=(const vtkOpenGLRenderWindow&);  // Not implemented.
};

#endif
