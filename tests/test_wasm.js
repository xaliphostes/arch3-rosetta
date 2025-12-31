import createpyarch3Module from './arch3.js';

const module = await createpyarch3Module();

const model = new module.Model()
model.setHalfSpace(false)

const surface = new module.Surface(model, [0,0,0, 1,0,0, 0,1,0], [0,1,2])
surface.setBcType("dip", "free")
surface.setBcType("strike", "free")
surface.setBcType("normal", "free")

const r = new module.UserRemote((x,y,z) => new module.Matrix33(0,0,0,0,0,-1))
model.addRemote(r)

const solver = new module.SeidelSolver(model)
solver.run()

const f = new module.Postprocess(model)
const displ = f.displAt(0,0,-2)
console.log(displ.x(), displ.y(), displ.z())

const stress = f.stressAt(0,0,-2)
console.log(stress.xx())


/* Usage (Browser)

<script src="pyarch3js.js"></script>
<script>
  pyarch3Module().then(m => console.log(m.listClasses()));
</script>
*/
