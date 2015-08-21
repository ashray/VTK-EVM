/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSmartVolumeMapper.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSmartVolumeMapper - Adaptive volume mapper
// .SECTION Description
// vtkSmartVolumeMapper is a volume mapper that will delegate to a specific
// volume mapper based on rendering parameters and available hardware. Use the
// SetRequestedRenderMode() method to control the behavior of the selection.
// The following options are available:
//
// .SECTION vtkSmartVolumeMapper::DefaultRenderMode
//          Allow the vtkSmartVolumeMapper to select the best mapper based on
//          rendering parameters and hardware support. If GPU ray casting is
//          supported, this mapper will be used for all rendering. If not,
//          then the vtkFixedPointRayCastMapper will be used exclusively.
//          This is the default requested render mode, and is generally the
//          best option. When you use this option, your volume will always
//          be rendered, but the method used to render it may vary based
//          on parameters and platform.
//
// .SECTION vtkSmartVolumeMapper::RayCastRenderMode
//          Use the vtkFixedPointVolumeRayCastMapper for both interactive and
//          still rendering. When you use this option your volume will always
//          be rendered with the vtkFixedPointVolumeRayCastMapper.
//
// .SECTION vtkSmartVolumeMapper::GPURenderMode
//          Use the vtkGPUVolumeRayCastMapper, if supported, for both
//          interactive and still rendering. If the GPU ray caster is not
//          supported (due to hardware limitations or rendering parameters)
//          then no image will be rendered. Use this option only if you have
//          already checked for supported based on the current hardware,
//          number of scalar components, and rendering parameters in the
//          vtkVolumeProperty.
//
//  You can adjust the contrast and brightness in the rendered image using the
//  FinalColorWindow and FinalColorLevel ivars. By default the
//  FinalColorWindow is set to 1.0, and the FinalColorLevel is set to 0.5,
//  which applies no correction to the computed image. To apply the window /
//  level operation to the computer image color, first a Scale and Bias
//  value are computed:
//  <pre>
//  scale = 1.0 / this->FinalColorWindow
//  bias  = 0.5 - this->FinalColorLevel / this->FinalColorWindow
//  </pre>
//  To compute a new color (R', G', B', A') from an existing color (R,G,B,A)
//  for a pixel, the following equation is used:
//  <pre>
//  R' = R*scale + bias*A
//  G' = G*scale + bias*A
//  B' = B*scale + bias*A
//  A' = A
//  </pre>
// Note that bias is multiplied by the alpha component before adding because
// the red, green, and blue component of the color are already pre-multiplied
// by alpha. Also note that the window / level operation leaves the alpha
// component unchanged - it only adjusts the RGB values.

#ifndef vtkSmartVolumeMapper_h
#define vtkSmartVolumeMapper_h

#include "vtkRenderingVolumeOpenGL2Module.h" // For export macro
#include "vtkVolumeMapper.h"
#include "vtkImageReslice.h" // for VTK_RESLICE_NEAREST, VTK_RESLICE_CUBIC

class vtkFixedPointVolumeRayCastMapper;
class vtkGPUVolumeRayCastMapper;
class vtkImageResample;
class vtkRenderWindow;
class vtkVolume;
class vtkVolumeProperty;

class VTKRENDERINGVOLUMEOPENGL2_EXPORT vtkSmartVolumeMapper : public vtkVolumeMapper
{
public:
  static vtkSmartVolumeMapper *New();
  vtkTypeMacro(vtkSmartVolumeMapper,vtkVolumeMapper);
  void PrintSelf( ostream& os, vtkIndent indent );

  // Description:
  // Set the final color window. This controls the contrast of
  // the image. The default value is 1.0. The Window can be
  // negative (this causes a "negative" effect on the image)
  // Although Window can be set to 0.0, any value less than
  // 0.00001 and greater than or equal to 0.0 will be set to
  // 0.00001, and any value greater than -0.00001 but less
  // than or equal to 0.0 will be set to -0.00001.
  // Initial value is 1.0.
  vtkSetMacro( FinalColorWindow, float );

  // Description:
  // Get the final color window. Initial value is 1.0.
  vtkGetMacro( FinalColorWindow, float );

  // Description:
  // Set the final color level. The level controls the
  // brightness of the image. The final color window will
  // be centered at the final color level, and together
  // represent a linear remapping of color values. The
  // default value for the level is 0.5.
  vtkSetMacro( FinalColorLevel,  float );

  // Description:
  // Get the final color level.
  vtkGetMacro( FinalColorLevel,  float );

//BTX
// The possible values for the default and current render mode ivars
  enum
  {
    DefaultRenderMode=0,
    RayCastAndTextureRenderMode,
    RayCastRenderMode,
    TextureRenderMode,
    GPURenderMode,
    UndefinedRenderMode,
    InvalidRenderMode
  };
//ETX

  // Description:
  // Set the requested render mode. The default is
  // vtkSmartVolumeMapper::DefaultRenderMode.
  void SetRequestedRenderMode(int mode);

  // Description:
  // Set the requested render mode to vtkSmartVolumeMapper::DefaultRenderMode.
  // This is the best option for an application that must adapt to different
  // data types, hardware, and rendering parameters.
  void SetRequestedRenderModeToDefault();

  // Description:
  // Set the requested render mode to vtkSmartVolumeMapper::RayCastRenderMode.
  // This option will use software rendering exclusively. This is a good option
  // if you know there is no hardware acceleration.
  void SetRequestedRenderModeToRayCast();

  // Description:
  // Set the requested render mode to vtkSmartVolumeMapper::GPURenderMode.
  // This option will use hardware accelerated rendering exclusively. This is a
  // good option if you know there is hardware acceleration.
  void SetRequestedRenderModeToGPU();

  // Description:
  // Get the requested render mode.
  vtkGetMacro( RequestedRenderMode, int );

  // Description:
  // This will return the render mode used during the previous call to
  // Render().
  int GetLastUsedRenderMode();

  // Description:
  // Value passed to the GPU mapper. Ignored by other mappers.
  // Maximum size of the 3D texture in GPU memory.
  // Will default to the size computed from the graphics
  // card. Can be adjusted by the user.
  // Useful if the automatic detection is defective or missing.
  vtkSetMacro( MaxMemoryInBytes, vtkIdType );
  vtkGetMacro( MaxMemoryInBytes, vtkIdType );

  // Description:
  // Value passed to the GPU mapper. Ignored by other mappers.
  // Maximum fraction of the MaxMemoryInBytes that should
  // be used to hold the texture. Valid values are 0.1 to
  // 1.0.
  vtkSetClampMacro( MaxMemoryFraction, float, 0.1f, 1.0f );
  vtkGetMacro( MaxMemoryFraction, float );

  // Description:
  // Set interpolation mode for downsampling (lowres GPU)
  // (initial value: cubic).
  vtkSetClampMacro(InterpolationMode, int,
                   VTK_RESLICE_NEAREST, VTK_RESLICE_CUBIC);
  vtkGetMacro(InterpolationMode, int);
  void SetInterpolationModeToNearestNeighbor();
  void SetInterpolationModeToLinear();
  void SetInterpolationModeToCubic();

  // Description:
  // This method can be used to render a representative view of the input data
  // into the supplied image given the supplied blending mode, view direction,
  // and view up vector.
  void CreateCanonicalView( vtkRenderer *ren,
                            vtkVolume *volume,
                            vtkVolume *volume2,
                            vtkImageData *image,
                            int blend_mode,
                            double viewDirection[3],
                            double viewUp[3] );

  // Description:
  // If AutoAdjustSampleDistances is on, the the ImageSampleDistance
  // will be varied to achieve the allocated render time of this
  // prop (controlled by the desired update rate and any culling in
  // use).
  vtkSetClampMacro( AutoAdjustSampleDistances, int, 0, 1 );
  vtkGetMacro( AutoAdjustSampleDistances, int );
  vtkBooleanMacro( AutoAdjustSampleDistances, int );

  // Description:
  // Set/Get the distance between samples used for rendering
  // when AutoAdjustSampleDistances is off, or when this mapper
  // has more than 1 second allocated to it for rendering.
  // Initial value is 1.0.
  vtkSetMacro( SampleDistance, float );
  vtkGetMacro( SampleDistance, float );


//BTX
  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // Initialize rendering for this volume.
  void Render( vtkRenderer *, vtkVolume * );

  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // Release any graphics resources that are being consumed by this mapper.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);
//ETX

protected:
  vtkSmartVolumeMapper();
  ~vtkSmartVolumeMapper();

  // Description:
  // Connect input of the vtkSmartVolumeMapper to the input of the
  // internal volume mapper by doing a shallow to avoid memory leaks.
  // \pre m_exists: m!=0
  void ConnectMapperInput(vtkVolumeMapper *m);

  // Description:
  // Connect input of the vtkSmartVolumeMapper to the input of the
  // internal resample filter by doing a shallow to avoid memory leaks.
  // \pre m_exists: f!=0
  void ConnectFilterInput(vtkImageResample *f);

  // Window / level ivars
  float   FinalColorWindow;
  float   FinalColorLevel;

  // GPU mapper-specific memory ivars.
  vtkIdType MaxMemoryInBytes;
  float MaxMemoryFraction;

  // Used for downsampling.
  int InterpolationMode;

  // The requested render mode is used to compute the current render mode. Note
  // that the current render mode can be invalid if the requested mode is not
  // supported.
  int     RequestedRenderMode;
  int     CurrentRenderMode;

  // Initialization variables.
  int          Initialized;
  vtkTimeStamp SupportStatusCheckTime;
  int          GPUSupported;
  int          RayCastSupported;
  int          LowResGPUNecessary;

  // This is the resample filter that may be used if we need to
  // create a low resolution version of the volume for GPU rendering
  vtkImageResample *GPUResampleFilter;

  // The initialize method. Called from ComputeRenderMode whenever something
  // relevant has changed.
  void  Initialize(vtkRenderer *ren,
                   vtkVolume *vol);

  // The method that computes the render mode from the requested render mode
  // based on the support status for each render method.
  void  ComputeRenderMode(vtkRenderer *ren,
                          vtkVolume *vol);

  // The three potential mappers
  vtkGPUVolumeRayCastMapper      *GPULowResMapper;
  vtkGPUVolumeRayCastMapper      *GPUMapper;
  vtkFixedPointVolumeRayCastMapper  *RayCastMapper;


  // We need to keep track of the blend mode we had when we initialized
  // because we need to reinitialize (and recheck hardware support) if
  // it changes
  int  InitializedBlendMode;

  // The distance between sample points along the ray
  float  SampleDistance;
  int    AutoAdjustSampleDistances;

private:
  vtkSmartVolumeMapper(const vtkSmartVolumeMapper&);  // Not implemented.
  void operator=(const vtkSmartVolumeMapper&);  // Not implemented.
};

#endif
