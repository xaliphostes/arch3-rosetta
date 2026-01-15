import arch3

model = arch3.Model()
surface = arch3.Surface(model, [0, 0, 0, 1, 0, 0, 0, 1, 0], [0, 1, 2])
surface.setBcType("dip", "free")
surface.setBcType("strike", "free")
surface.setBcType("normal", "free")
surface.setBcValue("normal", lambda x, y, z: -1200*9.81*abs(z))

r = arch3.UserRemote(lambda x,y,z: [0,0,0,0,0,-1])
model.addRemote(r)

r.setFunction(lambda x,y,z: [1,0.1,0,2,0,-1], True)

solver = arch3.SeidelSolver(model)
solver.run()

f = arch3.Postprocess(model)
displ = f.displAt(0,0,-2)
print(displ)

stress = f.stressAt(0,0,-2)
print(stress)
