## Game Description 
you play as the superb bird of paradise, and you must impress the girl of your dreams
you must diaplay your feathers, perform your dance, and always face the audience! 
Doing well and you might just get your shot! 

## Things to Implement
1. getting the mesh/scene in the game successfully 
2. move the male bird! (and female bird!)
3. animation (hopping, moving wings, maybe)
4. some scoring system

## better description of features
- press arrows to bobble up and down, and travel around the female bird. Heart on top of the female bird displaying how much they like you. 

- Animation hop, bobble(IK) should be from the base and propagate to the rest


## How does loading a scene work...
1.   Create a mesh data handler, use load function and [make_vao_for_program] to load data from [.pnct] files into [MeshBuffer]. NOTE: this contains exclusivly mesh info and not how they are composed
2. Load scene via LOAD function onto [Scene]. Contains interesting scene initializer...
3. (Into the scene initializer) very similar to [load] in *Scene.hpp*
"add transforms/objects/cameras from a scene file to this scene". You are basically **supplying the scene with the load function**. 
    You are also specifying a `Scene::Drawable` which "attaches attribute data to a transform". I *think* you can see this as describing to open GL on how should information be interpreted.

    all transforms are stored in `scene.transforms`
4. And how it should be loaded? 

## Data strucutres

## TODO
move the bird by getting a reference to the game object one wants to move

## GOAL 
have a scene, where there is a female bird, a male bird
the male bird can move in a circle and up and down

## Helpful
blender --background --python scenes/export-scene.py -- scenes/superb.blend dist/superb.scene

blender --background --python scenes/export-meshes.py -- scenes/superb.blend dist/superb.pnct

C:\Users\joyce\Documents\15466-CGP\CGP-2-DanceToImpress\
## to do this again...
have a data strucuture/tree of objects created at spawn
each transform should contain their base transform
do not hard code the translations and use the transformation matrices instead