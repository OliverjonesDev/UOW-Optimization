\## How to use this sheet



Open the console with the \*\*\\`\*\* key (backtick, under Esc) and type a command, then press Enter. Most commands toggle: run them again to turn them off.



\- \*\*Measure in Standalone, not just PIE.\*\* Play In Editor shares memory and CPU time with the editor, so loading and memory numbers are misleading. Use \*Play > Standalone Game\* for anything to do with memory or load times.

\- \*\*Make measurements fair.\*\* Turn off vsync and the frame cap first, or every scene looks like a steady 60 fps.

\- \*\*Change one thing at a time.\*\* Record the number before a fix and after it, in the same spot with the camera pointing the same way.

\- \*\*Commands are not case sensitive.\*\* `stat unit` and `STAT UNIT` are the same.



| Command | What it does |

| --- | --- |

| `t.MaxFPS 0` | Removes the frame rate cap |

| `r.VSync 0` | Turns vsync off |

| `stat none` | Hides every stat overlay at once |

| `pause` | Pauses the game, useful for reading numbers |

| `slomo 0.25` | Slows time down (1 = normal) |



\## Frame time overview



Start every investigation with `stat unit`: it tells you whether the game thread, render thread or GPU is the bottleneck. The largest of the three is what limits your frame rate.



| Command | What it shows | When to use it |

| --- | --- | --- |

| `stat fps` | Frames per second and frame time in ms | Quick check only. Always compare in \*\*ms\*\*, not fps |

| `stat unit` | Frame, Game (game thread), Draw (render thread), GPU and RHIT times in ms | First command in every investigation |

| `stat unitgraph` | The same numbers as a scrolling graph | Spotting hitches and slow climbs over time |

| `stat game` | Time spent in world tick, actor ticks, timers, navigation | Game thread is the bottleneck |

| `stat engine` | Engine-level counters such as frame time and primitive counts | General overview |

| `stat slow` | Only stats above a time threshold | Cutting through noise |



Read `stat unit` like this:



\- \*\*Game\*\* highest: the problem is C++ or Blueprint logic, ticking, physics queries or AI.

\- \*\*Draw\*\* highest: too many objects or draw calls for the render thread to prepare.

\- \*\*GPU\*\* highest: shading, lights, shadows, post processing or resolution.



\## Ticking and timers



Every actor and component that ticks costs game thread time every frame, even if its tick does nothing. These commands show what is ticking and how often.



| Command | What it shows |

| --- | --- |

| `dumpticks` | Every registered tick function, written to the Output Log |

| `dumpticks enabled` | Only the tick functions currently enabled |

| `dumpticks disabled` | Only the tick functions currently disabled |

| `dumpticks grouped` | Tick functions grouped by class, with counts |

| `ListTimers` | Every active timer, its rate and what it calls |

| `stat game` | Total time spent in ticks and timers |



Things to look for:



\- Classes that appear far more often than you expect.

\- Tick functions you can't explain: ask what would break if each one stopped.

\- Timer rates that don't match what the code is meant to do.



\## Rendering and GPU



Use these when `stat unit` shows \*\*Draw\*\* or \*\*GPU\*\* as the highest number.



| Command | What it shows |

| --- | --- |

| `stat gpu` | GPU time per rendering pass (shadows, lights, post processing, base pass) |

| `ProfileGPU` | A one-frame GPU capture in a window, broken down pass by pass (also \*\*Ctrl+Shift+,\*\*) |

| `stat scenerendering` | Draw calls, mesh draw calls, lights in the scene, render thread totals |

| `stat rhi` | Draw primitive calls, triangles drawn, render target memory |

| `stat initviews` | Visibility and culling cost, number of visible and occluded primitives |

| `stat lightrendering` | Cost of rendering lights |

| `stat shadowrendering` | Cost of rendering shadow maps |

| `r.ScreenPercentage 50` | Renders at half resolution. If GPU time drops a lot, you are limited by pixel cost |



View modes change how the scene is drawn so you can see the cost directly. They are also in the viewport's \*\*View Mode\*\* menu.



| Command | What it shows |

| --- | --- |

| `viewmode lit` | Back to normal |

| `viewmode unlit` | No lighting at all |

| `viewmode lightingonly` | Lighting without textures |

| `viewmode lightcomplexity` | How many lights touch each pixel (black = none, white = many) |

| `viewmode shadercomplexity` | How expensive each pixel's material is (green = cheap, red/white = expensive) |

| `viewmode wireframe` | Geometry only |



Show flags switch whole features off, so you can measure what they cost. Run the same command again to turn the feature back on.



| Command | Turns off |

| --- | --- |

| `show postprocessing` | All post processing |

| `show dynamicshadows` | Dynamic shadows |

| `show bloom` | Bloom |

| `show depthoffield` | Depth of field |

| `show ambientocclusion` | Screen-space ambient occlusion |

| `show staticmeshes` | Every static mesh |

| `show skeletalmeshes` | Every skeletal mesh |

| `show particles` | Particles |



\## Animation, physics and movement



| Command | What it shows |

| --- | --- |

| `stat anim` | Animation update and evaluation cost, number of skeletal meshes ticked |

| `a.URO.Draw 1` | Draws each character's animation update rate above it (Update Rate Optimization) |

| `stat collision` | Cost of traces, sweeps and overlap queries |

| `stat physics` | Physics simulation and scene query totals |

| `stat character` | Character movement cost (movement updates, sweeps, floor checks) |

| `stat ai` | AI controllers, behavior trees and perception |

| `stat navigation` | Navigation and pathfinding cost |

| `show collision` | Draws every collision shape in the world |



A useful test for animation: point the camera away from all characters. Work that only matters on screen should drop.



\## Memory, objects and loading



Run these in a \*\*Standalone Game\*\*. In PIE the editor has usually loaded assets already, so the numbers hide what the game itself loads.



| Command | What it shows |

| --- | --- |

| `obj list` | Every loaded object class, how many exist and how much memory they use |

| `obj list class=Texture2D` | Only one class (try `StaticMesh`, `SkeletalMesh`, `NiagaraSystem`, `MaterialInstanceDynamic`, or any of your own classes) |

| `obj gc` | Forces a garbage collection now |

| `memreport` | Writes a memory report to `Saved/Profiling/MemReports/` |

| `memreport -full` | A more detailed memory report. Take one before and one after a change and compare |

| `stat memory` | Memory used by the main engine systems |

| `stat memoryplatform` | Total physical and virtual memory used by the process |

| `stat streaming` | Texture streaming pool: how much is wanted, loaded and over budget |

| `ListTextures` | Every loaded texture with its size and memory, written to the Output Log |

| `ListStaticMeshes` | Every loaded static mesh with its memory, written to the Output Log |

| `stat llm` | Low Level Memory tracker summary by category (launch with `-llm`) |

| `stat llmfull` | The full LLM breakdown (launch with `-llm`) |



In the editor, also try \*\*Size Map\*\* and \*\*Reference Viewer\*\* (right-click any asset). They show everything an asset pulls into memory with it, without running the game.



\## Logging



Writing to the log costs time: every line is formatted and written to a file and to the Output Log. These commands control log categories while the game runs.



| Command | What it does |

| --- | --- |

| `log list` | Lists every log category and its current verbosity |

| `log <Category> off` | Silences a category, e.g. `log LogTemp off` |

| `log <Category> verbose` | Shows a category's verbose messages |

| `log <Category> log` | Back to the default verbosity |



If silencing a category changes the frame time, something is logging far too often.



\## Unreal Insights



Unreal Insights records every frame so you can find exactly which function is slow. Stat commands tell you \*that\* something is slow; Insights tells you \*what\*.



Open it from `Engine/Binaries/Win64/UnrealInsights.exe`, or from \*\*Tools > Unreal Insights\*\* in the editor.



\*\*Launch arguments.\*\* Add these to the Standalone Game's command line (\*Editor Preferences > Level Editor > Play > Additional Launch Parameters\*):



| Argument | What it records |

| --- | --- |

| `-trace=default` | CPU, GPU, frames, log and bookmarks |

| `-trace=default,memory` | The above plus every memory allocation |

| `-trace=default,loadtime,assetloadtime` | The above plus asset and package loading |

| `-statnamedevents` | Adds more detailed function names to the CPU track |

| `-llm` | Enables the Low Level Memory tracker |



\*\*Console commands while the game runs:\*\*



| Command | What it does |

| --- | --- |

| `Trace.File` | Starts recording to a file (optionally `Trace.File <Path> <Channels>`) |

| `Trace.Send localhost` | Streams the trace live to an open Unreal Insights |

| `Trace.Stop` | Stops recording |

| `Trace.Bookmark <Name>` | Drops a named marker on the timeline, e.g. before and after a test |

| `Trace.SnapshotFile` | Saves the last few seconds of trace to a file |

| `Trace.Status` | Shows whether tracing is on and which channels are enabled |

| `stat namedevents` | Turns detailed function names on or off |



In Insights, the \*\*Timing Insights\*\* tab shows the frame timeline and where time goes. The \*\*Asset Loading\*\* tab shows every package loaded and how long it took. \*\*Memory Insights\*\* shows allocations over time.



\## Commands built into this project



| Command | What it does |

| --- | --- |

| `Items.MeasureLoad <AssetPath>` | Loads one asset and reports the load time, packages, objects, item assets and memory that came in with it, broken down by class. Example: `Items.MeasureLoad /Game/UOW\_Custom/DataAssets/DA\_Ingre\_Flowers` |

| `Debug.DrawInteraction 1` | Draws the player's interaction check and lists what it is hitting on screen. `0` turns it off |



`Items.MeasureLoad` only measures assets that are not already in memory. If it says the asset is already loaded, run `obj gc` and try again, or measure from an empty map in a Standalone Game.



\## Suggested workflow



1\. Turn off the frame cap and vsync: `t.MaxFPS 0`, `r.VSync 0`.

2\. Run `stat unit` and note which of Game, Draw or GPU is highest.

3\. Narrow it down with the matching group: `stat game` and `dumpticks` for Game, `stat scenerendering` and `stat initviews` for Draw, `stat gpu` and `ProfileGPU` for GPU.

4\. Record a trace with Unreal Insights and find the function or pass that costs the most.

5\. Read the code or asset responsible and form one theory about why it is slow.

6\. Fix one thing, then measure again in the same spot. Write down the before and after numbers in ms.

7\. Repeat. Also check the editor itself (not just Play), the player character, memory and loading, and what changes the longer the game runs.



