/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPeriodicFiler.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkAngularPeriodicFilter - A filter to produce mapped angular periodic
// multiblock dataset from a single block, by rotation.
//
// .SECTION Description:
// Generate angular periodic dataset by rotating points, vectors and tensors
// data arrays from an original data array.
// The generated dataset is of the same type than the input (float or double).
// To compute the rotation this filter needs
// i) a number of periods, wich can be the maximum, i.e. a full period,
// ii) an angle, wich can be fetched from a field data array in radian or directly
// in degrees; iii) the axis (X, Y or Z) and the center of rotation.
// Point coordinates are transformed, as well as all vectors (3-components) and
// tensors (9 components) in points and cell data arrays.
// The generated multiblock will have the same tree architecture than the input,
// except transformed leaves are replaced by a vtkMultipieceDataSet.
// Supported input leaf dataset type are: vtkPolyData, vtkStructuredGrid
// and vtkUnstructuredGrid. Other data objects are rotated using the
// transform filter (at a high cost!).

#ifndef vtkAngularPeriodicFilter_h
#define vtkAngularPeriodicFilter_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkPeriodicFilter.h"

class vtkDataSetAttributes;
class vtkMultiPieceDataSet;
class vtkPointSet;

#define VTK_ROTATION_MODE_DIRECT_ANGLE 0  // Use user-provided angle
#define VTK_ROTATION_MODE_ARRAY_VALUE  1  // Use array from input data as angle

class VTKFILTERSGENERAL_EXPORT vtkAngularPeriodicFilter : public vtkPeriodicFilter
{
public:
  static vtkAngularPeriodicFilter* New();
  vtkTypeMacro(vtkAngularPeriodicFilter, vtkPeriodicFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get The rotation mode.
  // VTK_ROTATION_MODE_DIRECT_ANGLE to specifiy a angle value (default),
  // VTK_ROTATION_MODE_ARRAY_VALUE to use value from an array in the input dataset.
  vtkSetClampMacro(RotationMode, int,
    VTK_ROTATION_MODE_DIRECT_ANGLE, VTK_ROTATION_MODE_ARRAY_VALUE);
  vtkGetMacro(RotationMode, int);
  void SetRotationModeToDirectAngle()
    { this->SetRotationMode(VTK_ROTATION_MODE_DIRECT_ANGLE); }
  void SetRotationModeToArrayValue()
    { this->SetRotationMode(VTK_ROTATION_MODE_ARRAY_VALUE); }

  // Description:
  // Set/Get Rotation angle, in degrees.
  // Used only with VTK_ROTATION_MODE_DIRECT_ANGLE.
  // Default is 180.
  vtkSetMacro(RotationAngle, double);
  vtkGetMacro(RotationAngle, double);

  // Description:
  // Set/Get Name of array to get the angle from.
  // Used only with VTK_ROTATION_MODE_ARRAY_VALUE.
  vtkSetStringMacro(RotationArrayName);
  vtkGetStringMacro(RotationArrayName);

  // Description:
  // Set/Get Rotation Axis, 0 for X, 1 for Y, 2 for Z
  vtkSetClampMacro(RotationAxis, int, 0, 2);
  vtkGetMacro(RotationAxis, int);
  void SetRotationAxisToX();
  void SetRotationAxisToY();
  void SetRotationAxisToZ();

  // Description:
  // Set/Get Rotation Center
  vtkSetVector3Macro(Center, double);
  vtkGetVector3Macro(Center, double);

protected:
  vtkAngularPeriodicFilter();
  ~vtkAngularPeriodicFilter();

  // Description:
  // Create a transform copy of the provided data array
  vtkDataArray* TransformDataArray(vtkDataArray* inputArray,
                                   double angle,
                                   bool useCenter = true,
                                   bool normalize = false);

  // Description:
  // Append a periodic piece to dataset, by computing rotated mesh and data
  void AppendPeriodicPiece(double angle, vtkIdType iPiece,
                           vtkDataObject* inputNode,
                           vtkMultiPieceDataSet* multiPiece);

  // Description:
  // Compute periodic pointset, rotating point, using provided angle
  void ComputePeriodicMesh(vtkPointSet* dataset, vtkPointSet* rotatedDataset,
                           double angle);

  // Description:
  // Compute periodic point/cell data, using provided angle
  void ComputeAngularPeriodicData(vtkDataSetAttributes* data,
                                  vtkDataSetAttributes* rotatedData,
                                  double angle);

  // Description:
  // Create a periodic data, leaf of the tree
  void CreatePeriodicDataSet(vtkCompositeDataIterator* loc,
                             vtkCompositeDataSet* output,
                             vtkCompositeDataSet* input);

private:
  vtkAngularPeriodicFilter(const vtkAngularPeriodicFilter&); // Not implemented.
  void operator=(const vtkAngularPeriodicFilter&); // Not implemented.

  int RotationMode;
  char* RotationArrayName;  // user-provided array name to use as angle, for ROTATION_MODE_ARRAY_VALUE

  // Transform parameters
  double RotationAngle;
  int RotationAxis;         // Axis to rotate around, 0 for X, 1 for Y, 2 for Z
  double Center[3];         // Center of rotation
};

#endif
