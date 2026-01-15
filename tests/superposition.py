import arch3
import numpy as np

model = arch3.Model()

hasPressure = True
n_points = 1000

surface = arch3.Surface(model, [0, 0, 0, 1, 0, 0, 0, 1, -0.1], [0, 1, 2])
surface.setBcType("dip", "free")
surface.setBcType("strike", "free")
surface.setBcType("normal", "free")

remote = arch3.UserRemote(lambda x,y,z: [0,0,0,0,0,1])
model.addRemote(remote)

solver = arch3.SeidelSolver(model)
postprocess = arch3.Postprocess(model)

obs_points = np.random.uniform(-10, 10, n_points * 3)

nb_simulations = 6 if hasPressure == True else 4

# Storage for results: list of (displ_array, stress_array) per simulation
all_displ = []  # shape will be (nb_simulations, n_obs * 3)
all_stress = [] # shape will be (nb_simulations, n_obs * 6)

def do_simulation(index):
    alpha = [1 if i == (index - 1) else 0 for i in range(nb_simulations)]

    # Setup the new remote
    remote.setFunction(lambda x, y, z, a=alpha: [
        a[0] * abs(z), a[1] * abs(z), 0,
        a[2] * abs(z), 0,
        a[3] * abs(z)
    ], True)

    # Setup the bc value for the surface
    surface.setBcValue("normal", lambda x, y, z, a=alpha: a[5 + index - 1] if (5 + index - 1) < len(a) and hasPressure else 0)

    solver.run()

    all_displ.append(postprocess.displ(obs_points))
    all_stress.append(postprocess.stress(obs_points))
    print(f"Simulation {index}/{nb_simulations} completed")

for i in range(1, nb_simulations + 1):
    do_simulation(i)

# Convert to numpy arrays
all_displ = np.array(all_displ)   # (nb_simulations, n_obs * 3)
all_stress = np.array(all_stress) # (nb_simulations, n_obs * 6)

# Save displacement file
# Format: # x y z displ1x displ1y displ1z displ2x displ2y displ2z ... displ6x displ6y displ6z
with open("superposition_displ.txt", "w") as fout:
    # Header
    header = "# id x y z"
    for i in range(nb_simulations):
        header += f" displ{i+1}x displ{i+1}y displ{i+1}z"
    fout.write(header + "\n")

    # Data
    n_obs = len(obs_points) // 3
    for j in range(n_obs):
        x, y, z = obs_points[j*3], obs_points[j*3 + 1], obs_points[j*3 + 2]
        line = f"{j} {x} {y} {z}"
        for i in range(nb_simulations):
            dx = all_displ[i, j*3]
            dy = all_displ[i, j*3 + 1]
            dz = all_displ[i, j*3 + 2]
            line += f" {dx} {dy} {dz}"
        fout.write(line + "\n")

# Save stress file
# Format: # x y z stress1xx stress1xy stress1xz stress1yy stress1yz stress1zz stress2xx ...
with open("superposition_stress.txt", "w") as fout:
    # Header
    header = "# id x y z"
    for i in range(nb_simulations):
        header += f" stress{i+1}xx stress{i+1}xy stress{i+1}xz stress{i+1}yy stress{i+1}yz stress{i+1}zz"
    fout.write(header + "\n")

    # Data
    n_obs = len(obs_points) // 3
    for j in range(n_obs):
        x, y, z = obs_points[j*3], obs_points[j*3 + 1], obs_points[j*3 + 2]
        line = f"{j} {x} {y} {z}"
        for i in range(nb_simulations):
            for k in range(6):
                s = all_stress[i, j*6 + k]
                line += f" {s}"
        fout.write(line + "\n")

print("Results saved to superposition_displ.txt and superposition_stress.txt")
