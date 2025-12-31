import binding from './index.js';

const model = new binding.Model()
model.setHalfSpace(false)

const positions = new Float64Array([0, 0, 0, 1, 0, 0, 0, 1, 0]);
const indices = new Int32Array([0, 1, 2]);
const surface = new binding.Surface(model, positions, indices)
surface.setBcType("dip", "free")
surface.setBcType("strike", "free")
surface.setBcType("normal", "free")

const r = new binding.UserRemote((x,y,z) => new binding.Matrix33(0,0,0,0,0,-1))
model.addRemote(r)

const solver = new binding.SeidelSolver(model)
solver.run()

const f = new binding.Postprocess(model)
const displ = f.displAt(0,0,-2)
console.log(displ.x(), displ.y(), displ.z())

const stress = f.stressAt(0,0,-2)
console.log(stress.xx(), stress.xy(), stress.xz(), stress.yy(), stress.yz(), stress.zz())