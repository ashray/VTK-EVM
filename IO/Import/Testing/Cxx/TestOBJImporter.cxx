
#include "vtkVRMLImporter.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkOBJImporter.h"
#include "vtkTestUtilities.h"
#include "vtkNew.h"
#include "vtkJPEGWriter.h"
#include "vtkPNGWriter.h"
#include "vtkImageCanvasSource2D.h"
#include "vtkImageCast.h"

#include "vtkTestUtilities.h"
#include "vtksys/SystemTools.hxx"

namespace
{
int s_interactive = 0;

bool bInteractive()
{
    return (s_interactive>0);
}

}

int TestOBJImporter( int argc, char * argv [] )
{
    // Files for testing demonstrate updated functionality for OBJ import:
    //       polydata + textures + actor properties all get loaded.

    if(argc < 8)
    {
      std::cerr<<"expected TestName -D  File1.obj File2.obj.mtl  texture1  texture2  ... "<<std::endl;
      return -1;
    }

    std::string filenameOBJ(argv[2]);
    std::string filenameMTL(argv[3]);

    std::string texfile1(argv[4]);
    std::string texfile2(argv[5]);
    std::vector<std::string> tmp1,tmp2;
    std::string texture_path1 = vtksys::SystemTools::GetFilenamePath(texfile1);
    std::string texture_path2 = vtksys::SystemTools::GetFilenamePath(texfile2);
    if( 0 != texture_path1.compare(texture_path2) )
    {
      std::cerr<<" texture files should be in the same path: "<<texture_path1<<std::endl;
      return -2;
    }

    std::string tmppath(argv[7]);
    vtkNew<vtkOBJImporter> importer;

    if(argc > 8)
      s_interactive = 1;

    importer->SetFileName(filenameOBJ.data());
    importer->SetFileNameMTL(filenameMTL.data());
    importer->SetTexturePath(texture_path1.data());

    vtkNew<vtkRenderer> ren;
    vtkNew<vtkRenderWindow> renWin;
    vtkNew<vtkRenderWindowInteractor> iren;

    renWin->AddRenderer(ren.Get());
    iren->SetRenderWindow(renWin.Get());
    importer->SetRenderWindow(renWin.Get());
    importer->Update();

    ren->ResetCamera();

    if( 1 > ren->GetActors()->GetNumberOfItems() )
    {
      std::cerr << "failed to get an actor created?!" << std::endl;
      return -1;
    }
    if( bInteractive() )
    {
        renWin->SetSize(800,600);
        renWin->SetAAFrames(3);
        iren->Start();
    }
    return 0;
}
