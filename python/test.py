import pyarch3 as ARCH

model = ARCH.Model()

# print(1)
surface = ARCH.Surface(model, [0,0,0, 1,0,0, 0,1,-1], [0,1,2])
# print(2)
surface.setBcType("dip", "free")
surface.setBcType("strike", "free")
surface.setBcType("normal", "fixed")

model.setHalfSpace(True)
# print(3)
print("nb dofs:", model.nbDof())
# print(4)

remote = ARCH.RemoteStress()
remote.seth(1)
remote.setH(2)
remote.setv(3)
remote.setTheta(23)
# print(5)
model.addRemote(remote)
# print(6)

solver = ARCH.SeidelSolver(model)
# print(7)
solver.run()
# print(8)
print(surface.displ())
