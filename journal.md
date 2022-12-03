# Journal

### Day 0 (2022-12-03)

Start of journal. It's been a year working on this engine and I think it's good to start documenting logging rants and thoughts as I go back and forth about ideas within the context of this engine.

Right now, I'm trying to see if I can clearly seperate all my code into 3 parts:
* The part that is the ever changing 'game' that runs on the engine, which I will prefix with 'scene'. Code here is VERY replaceable and I don't care as much what's going on in there from an engine developer point of view
* The part that is 'platform agnostic', which I will prefix with my personal alias 'momo'. This is equivalent of my 'std' library.
* The part that is the meat of the engine code that runs 'scenes'. This is prefixed with 'moe' which stands for 'My Own Engine'.

Before, 'scene' and 'engine' is one thing, which works, but it would be nice to identify what are game-specific code and what are engine-specific code so that I can have the right mindset when I'm working with either.

The first thing I identified to change is the state management. Right now, what scenes there are is right in the core of the engine, which is definitely wrong with this mentality going forward. We will have to figure out how to move the 'knowledge of what scenes there are' away from the engine layer.


