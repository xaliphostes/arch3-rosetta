import pyarch3 as ARCH

model = ARCH.Model()
print("------------1")

surface = ARCH.Surface(model, [0,0,0, 1,0,0, 0,1,0], [0,1,2])
print("------------2")
surface.setBcType("dip", "free")
surface.setBcType("strike", "free")
surface.setBcType("normal", "fixed")


# model.addSurface(surface)
model.setHalfspace(True)
print(model.nbDof())

remote = ARCH.AndersonianRemote()
remote.seth("1")
remote.setH("2")
remote.setv("3")
remote.setTheta(23)
model.addRemote(remote)