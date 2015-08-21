vtk_module(vtkRenderingOpenGL
  IMPLEMENTS
    vtkRenderingCore
  BACKEND
    OpenGL
  PRIVATE_DEPENDS
    # These are likely to be removed soon - split Rendering/OpenGL further.
    vtkImagingHybrid # For vtkSampleFunction
    vtksys
  COMPILE_DEPENDS
    vtkParseOGLExt
    vtkUtilitiesEncodeString
  TEST_DEPENDS
    vtkInteractionStyle
    vtkTestingRendering
    vtkIOExport
    vtkIOLegacy
    vtkIOXML
    vtkRenderingImage
    vtkRenderingLOD
    vtkImagingGeneral
    vtkImagingSources
    vtkFiltersProgrammable
    vtkFiltersSources
    vtkRenderingAnnotation
  KIT
    vtkOpenGL
  )
