# arch3 Python API Reference

This document describes the Python bindings for the arch3 library, a boundary element method (BEM) library for geomechanical modeling.

## Table of Contents

- [Model](#model)
- [Surface](#surface)
- [Remote Stress Classes](#remote-stress-classes)
  - [BaseRemote](#baseremote)
  - [RemoteStress](#remotestress)
  - [UserRemote](#userremote)
- [Temperature Field](#temperaturefield)
- [Solvers](#solvers)
  - [IterativeSolver](#iterativesolver)
  - [SeidelSolver](#seidelsolver)
- [Postprocess](#postprocess)

---

## Model

The `Model` class is the central container for a BEM simulation. It holds surfaces, material properties, remote stress conditions, and temperature fields.

### Constructor

```python
model = arch3.Model()
```

### Methods

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `setHalfSpace(enabled)` | `enabled: bool` | `None` | Enable half-space (free surface at z=0) or whole-space formulation |
| `setMaterial(poisson, young, density)` | `poisson: float`, `young: float`, `density: float` | `None` | Set homogeneous material properties for the model |
| `addRemote(remote)` | `remote: BaseRemote` | `None` | Add a remote stress condition to the model |
| `nbDof()` | - | `int` | Return the total number of degrees of freedom in the model |
| `setTemperatureField(field)` | `field: TemperatureField` | `None` | Assign a temperature field to the model |
| `setThermalExpansion(alpha)` | `alpha: float` | `None` | Set the coefficient of thermal expansion |

### Typical values for the coefficient of linear thermal expansion (α) in geomechanics:

  | Material      | α (per °C)   |
  |---------------|--------------|
  | Granite       | 7–9 × 10⁻⁶   |
  | Basalt        | 5–8 × 10⁻⁶   |
  | Sandstone     | 10–12 × 10⁻⁶ |
  | Limestone     | 4–8 × 10⁻⁶   |
  | Shale         | 8–12 × 10⁻⁶  |
  | Marble        | 4–7 × 10⁻⁶   |
  | Quartzite     | 11–13 × 10⁻⁶ |
  | Salt (halite) | 35–40 × 10⁻⁶ |

### Example

```python
import arch3

# Create model
model = arch3.Model()
model.setHalfSpace(True)
model.setMaterial(0.25, 30e9, 2700)  # poisson, young, density

# Add remote stress
remote = arch3.RemoteStress()
remote.seth(-10e6)  # sigma_h
remote.setH(-15e6)  # sigma_H
remote.setv(-20e6)  # sigma_v
model.addRemote(remote)
```

---

## Surface

The `Surface` class represents a triangulated fault or fracture surface in the model.

### Constructor

```python
surface = arch3.Surface(model, positions, indices)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `model` | `Model` | Parent model that owns this surface |
| `positions` | `list[float]` | Flat array of vertex coordinates `[x0, y0, z0, x1, y1, z1, ...]` |
| `indices` | `list[int]` | Flat array of triangle indices `[i0, j0, k0, i1, j1, k1, ...]` |

### Methods

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `setBcType(axis, type)` | `axis: str`, `type: str` | `None` | Set boundary condition type for an axis |
| `setBcValue(axis, function)` | `axis: str`, `function: callable` | `None` | Set boundary condition value using a function `f(x, y, z) -> float` |
| `setBcValues(values)` | `values: list[float]` | `bool` | Set boundary condition values directly as a flat array |

### Boundary Condition Types

The `axis` parameter can be:
- `"x"` or `"strike"` - Strike-slip direction
- `"y"` or `"dip"` - Dip-slip direction
- `"z"` or `"normal"` - Normal direction

The `type` parameter can be:
- `"0"`, `"t"`, `"traction"`, `"neumann"`, `"unknown"` or `"free"` - Traction-free (stress boundary condition = 0)
- `"1"`, `"b"`, `"displ"`, `"fixed"`, `"dirichlet"`, `"locked"` or `"displacement"` - Prescribed displacement

### Example

```python
# Create a simple triangular surface
positions = [0, 0, -1000,   1000, 0, -1000,   500, 1000, -2000]  # 3 vertices
indices = [0, 1, 2]  # 1 triangle

surface = arch3.Surface(model, positions, indices)

# Set as a free-slip fault (zero shear traction, no opening)
surface.setBcType("strike", "free")
surface.setBcType("dip", "free")
surface.setBcType("normal", "displacement")
surface.setBcValue("normal", lambda x, y, z: 0.0)
```

---


## Remote Stress Classes

Remote stress classes define the far-field stress state applied to the model.

### BaseRemote

Base class for all remote stress types. Not typically instantiated directly.

### RemoteStress

Defines a uniform Andersonian remote stress field using principal stresses.

#### Constructor

```python
remote = arch3.RemoteStress()
```

#### Methods

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `seth(value)` | `value: float` | `None` | Set minimum horizontal stress (sigma_h) |
| `setH(value)` | `value: float` | `None` | Set maximum horizontal stress (sigma_H) |
| `setv(value)` | `value: float` | `None` | Set vertical stress (sigma_v) |
| `setTheta(angle)` | `angle: float` | `None` | Set orientation angle of sigma_H from x-axis (radians) |

#### Example

```python
remote = arch3.RemoteStress()
remote.seth(-10e6)   # sigma_h = -10 MPa (compression is negative)
remote.setH(-15e6)   # sigma_H = -15 MPa
remote.setv(-25e6)   # sigma_v = -25 MPa
remote.setTheta(0.5) # sigma_H oriented 0.5 rad from x-axis

model.addRemote(remote)
```

### UserRemote

Defines a custom spatially-varying remote stress field using a user-provided function.

#### Constructor

```python
remote = arch3.UserRemote(stress_function)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `stress_function` | `callable` | Function `f(x, y, z) -> [sxx, sxy, sxz, syy, syz, szz]` returning 6 stress components |

#### Methods

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `setFunction(func)` | `func: callable` | `None` | Set a new stress function |

#### Example

```python
def lithostatic_stress(x, y, z):
    """Lithostatic stress increasing with depth"""
    rho = 2700  # density kg/m^3
    g = 9.81    # gravity m/s^2
    sv = rho * g * abs(z)  # vertical stress
    k = 0.5     # horizontal/vertical stress ratio
    sh = k * sv
    # Return [sxx, sxy, sxz, syy, syz, szz]
    return [-sh, 0, 0, -sh, 0, -sv]

remote = arch3.UserRemote(lithostatic_stress)
model.addRemote(remote)
```

---

## TemperatureField

The `TemperatureField` class defines temperature distribution for thermo-elastic analysis.

### Constructor

```python
temp_field = arch3.TemperatureField()
```

### Methods

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `setConstant(temperature)` | `temperature: float` | `None` | Set a uniform temperature field |
| `setFunction(func)` | `func: callable` | `bool` | Set temperature as function `f(x, y, z) -> float` |
| `setReferenceTemperature(temp)` | `temp: float` | `None` | Set reference temperature (zero thermal strain) |
| `setActive(active)` | `active: bool` | `None` | Enable or disable temperature effects |
| `getTemperature(position)` | `position: tuple[float, float, float]` | `float` | Get temperature at a point |
| `getTemperatureGradient(position)` | `position: tuple[float, float, float]` | `tuple` | Get temperature gradient at a point |
| `isActive()` | - | `bool` | Check if temperature effects are active |

### Example

```python
# Geothermal gradient: temperature increasing with depth
def geothermal(x, y, z):
    surface_temp = 20.0  # degrees C
    gradient = 0.03      # 30 deg/km = 0.03 deg/m
    return surface_temp - gradient * z  # z is negative for depth

temp_field = arch3.TemperatureField()
temp_field.setFunction(geothermal)
temp_field.setReferenceTemperature(20.0)
temp_field.setActive(True)

model.setTemperatureField(temp_field)
model.setThermalExpansion(1e-5)  # coefficient of thermal expansion
```

---

## Solvers

### SeidelSolver

Gauss-Seidel iterative solver for the BEM system.

#### Constructor

```python
solver = arch3.SeidelSolver(model)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `model` | `Model` | The model to solve |

#### Methods

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `setEps(eps)` | `eps: float` | `None` | Set the precison of the solver (1e-9) |
| `setMaxIter(nb)` | `nb: int` | `None` | Set maximum number of iterations for the solver (200) |
| `run()` | - | `bool` | Execute the solver; returns `True` on convergence |

#### Example

```python
solver = arch3.SeidelSolver(model)
success = solver.run()

if success:
    print("Solver converged!")
else:
    print("Solver did not converge")
```

---

## Postprocess

The `Postprocess` class computes displacement, strain, and stress fields after solving.

### Constructor

```python
post = arch3.Postprocess(model)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `model` | `Model` | The solved model |

### Methods

#### Field computation at observation points

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `displ(positions)` | `positions: list[float]` | `list[float]` | Compute displacements at points `[x0,y0,z0, ...]` -> `[ux0,uy0,uz0, ...]` |
| `strain(positions)` | `positions: list[float]` | `list[float]` | Compute strains at points -> `[exx,exy,exz,eyy,eyz,ezz, ...]` |
| `stress(positions)` | `positions: list[float]` | `list[float]` | Compute stresses at points -> `[sxx,sxy,sxz,syy,syz,szz, ...]` |

#### Single point evaluation

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `displAt(x, y, z)` | `x, y, z: float` | `tuple[float, float, float]` | Displacement at a single point |
| `strainAt(x, y, z)` | `x, y, z: float` | `tuple[float, ...]` | Strain tensor (6 components) at a single point |
| `stressAt(x, y, z)` | `x, y, z: float` | `tuple[float, ...]` | Stress tensor (6 components) at a single point |

#### Fault surface results

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `burgers(local, atTriangles)` | `local: bool`, `atTriangles: bool` | `list[float]` | Get slip (Burgers vector) on surfaces |
| `burgersPlus(local, atTriangles)` | `local: bool`, `atTriangles: bool` | `list[float]` | Get slip on positive side of surfaces |
| `burgersMinus(local, atTriangles)` | `local: bool`, `atTriangles: bool` | `list[float]` | Get slip on negative side of surfaces |
| `residualTractions()` | - | `list[float]` | Get residual tractions (should be near zero) |

The `local` parameter controls coordinate system:
- `True`: Results in local fault coordinates (strike, dip, normal)
- `False`: Results in global coordinates (x, y, z)

The `atTriangles` parameter controls sampling location:
- `True`: Values at triangle centers
- `False`: Values at vertices

### Example

```python
# After solving
solver = arch3.SeidelSolver(model)
solver.run()

# Create postprocessor
post = arch3.Postprocess(model)

# Compute displacements on a grid
import numpy as np
x = np.linspace(-5000, 5000, 50)
y = np.linspace(-5000, 5000, 50)
z = -100  # 100m depth

positions = []
for xi in x:
    for yi in y:
        positions.extend([xi, yi, z])

displacements = post.displ(positions)

# Get slip on faults in local coordinates
slip = post.burgers(local=True, atTriangles=True)

# Single point evaluation
ux, uy, uz = post.displAt(0, 0, -500)
print(f"Displacement at origin: ({ux}, {uy}, {uz})")
```

---

## Complete Example

```python
import arch3

# 1. Create model
model = arch3.Model()
model.setHalfSpace(True)
model.setMaterial(0.25, 30e9, 2700)

# 2. Create a fault surface (simple rectangle)
# Vertices: 4 corners of a 2km x 2km fault dipping 60 degrees
import math
dip = math.radians(60)
positions = [
    -1000, 0, -500,           # bottom-left
     1000, 0, -500,           # bottom-right
     1000, 2000*math.cos(dip), -500 - 2000*math.sin(dip),  # top-right
    -1000, 2000*math.cos(dip), -500 - 2000*math.sin(dip),  # top-left
]
indices = [0, 1, 2, 0, 2, 3]  # 2 triangles

surface = arch3.Surface(model, positions, indices)
surface.setBcType("strike", "free")
surface.setBcType("dip", "free")
surface.setBcType("normal", "displacement")
surface.setBcValue("normal", lambda x, y, z: 0.0)

# 3. Set remote stress
remote = arch3.RemoteStress()
remote.seth(-10e6)
remote.setH(-15e6)
remote.setv(-25e6)
model.addRemote(remote)

# 4. Solve
solver = arch3.SeidelSolver(model)
if solver.run():
    print("Solver converged!")

    # 5. Post-process
    post = arch3.Postprocess(model)

    # Get slip on fault
    slip = post.burgers(local=True, atTriangles=True)
    print(f"Slip components: {slip}")

    # Compute surface displacement
    obs_points = [0, 100, 0, 0, 200, 0, 0, 500, 0]
    surface_displ = post.displ(obs_points)
    print(f"Surface displacements: {surface_displ}")
else:
    print("Solver failed to converge")
```

---

## Conventions

### Sign Conventions
- **Stress**: Compression is **negative**, tension is positive
- **Coordinates**: Right-handed coordinate system with z typically positive upward (depth is negative z)
- **Slip**: Positive slip follows the local coordinate system convention on the fault surface

### Local csys Conventions

#### Convention for **Arch** is **Okada** for local csys, i.e.:
- x is normal
- y is strike
- z is dip (pointing upward on fault plane)

#### Recall that convention for **Poly3D** is:
- x is dip (pointing downward on fault plane)
- y is strike
- z is normal

#### Transformation from Poly3D -> Okada
- switch x and z coordinate for `displacement`, `traction` and `bcType`
- negate the new z coordinate for `displacement` and `traction` only

#### Transformation from Okada -> Poly3D
- switch x and z coordinate for `displacement`, `traction` and `bcType`
- negate the new x coordinate for `displacement` and `traction` only