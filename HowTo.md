# Required Libraries #

  * OpenGL
  * Cg
  * CgGL
  * GLEW
  * GLUT (Test project only)

# Demonstration #

  * Download the latest stable release
  * Unzip to desired location
  * Run "bin/TestFluidic.exe" for a demonstration of the capabilities

# How to Use #

Easiest way is to look at the Test project, should be pretty straight forward.

You will need to include `Fluidic.h` to use it. Instantiate and use as follows:

```
FluidOptions options;
options.Size = Vector(1, 1);
options.RenderResolution = Vector(400, 400);
options.SolverResolution = Vector(200, 200);
options.SolverOptions = RS_NICE;
options.Viscosity = ViscosityAir;

Fluid *fluid = new Fluid2D("../path/to/cgfiles/");
fluid->Init(options);
fluid->SetColorDensities(1, 0, -1);
```

Note: in the trunk copy, `FluidOptions options = Fluid2D::DefaultOptions()` can be used instead of setting all the options manually.

Updating is simply `fluid->Update(dt);`, and rendering is `fluid->Render()`. Note that for the 3D version, the RenderResolution should match the viewport size (set by `glViewport`), or the results may be erroneous.

The Fluid will render in the size defined by `FluidOptions::Size`