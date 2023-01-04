# Journal

### 2022-12-28
I have added gists, which are snippets of code that we can reuse. 
I think that's the best way I can get around writing data structures that are meaningful to a project, as opposed to using 'generic' data structures that I might be forced to think around them for the sake of ease.

### 2022-12-27
I'm having second thoughts about using macros for data structures. 
The main issue with macros is that it severely restricts the API design and that it forces me to design my code a certain way, which isn't neccesarily good.

One would argue that having a generic data structure will help in prototyping and iteration. 
This is...true, but the issue is that the amount of work to refactor away from it takes time too, and I'm not certain that those times add up enough to have a negative impact.  

Another issue is readability. 
Writing 'al_append' ensures that I'm pushing back to some sort of array list. 
It is worth sacrificing the readability?

Maybe it's better to just remove it all and just write data structures that are performant anyways.

...

Just done refactoring. 
Struct declarations seems 'cleaner' without all the intemediary stuff.
Implementation seems a little messier.
Should've probably not been lazy and wrote helper functions.

Either way, it's comfortaing to see that I only used lists in the (game) layer.
I had thought that I would use it in more critical places but turns out that's not the case.
Maybe generic data structures are really overhyped after all?


### 2022-12-26
I feel like I have reached the limit of using C in my (momo) layer. 
It has reached the point where I am spending too much time trying to play around the lack of C++ features, like using goto instead of the defer keyword I have. 
Elevating a system from (moe) layer to (momo) layer is already very tedious because of this.
As such, I am now allowing (momo) layer to be C++ compatible. 
It's not like I tested otherwise anyway.

### 2022-12-08
I have been looking into the platform input system recently and felt that I need to re-evaluate what's going on there.

Right now, my understanding of the workflow for platform input is:
- platform_t API states what kind of input the engine accepts
- Concrete platform layer will then implement the API

This creates inflexibility on the 'Scene' layer.
Basically, we can't really iterate over the 'best controls' for the 'Scene' layer.

A solution to this is simply to do it the traditional way; expose all possible controls/keycodes and let the Scene layer handle it.
A big issue I have with this is where the code for switching between different devices is going to be.
Logically, it's not going to be within Scene; Scene shouldn't care about whether there is a change of device and all that as much as possible. If anything, I still strongly believe that the platform layer should handle it.

This just means that I'm not going to change my current design. 
If anything, I should start to slowly clamp down on exactly what kind of input I am allowing my game to have.

Remapping is still something I will need to tackle at some point though. 
Perhaps a good idea is to allow remapping at the concrete platform layer via reading in a text file?

### 2022-12-03

Start of journal. 
It's been solid 2 years working on this engine, with the first year  and I think it's good to start documenting logging rants and thoughts as I go back and forth about ideas within the context of this engine.

Right now, I'm trying to see if I can clearly seperate all my code into 3 parts:

* Scene - The part that is the ever changing 'game' that runs on the engine, which I will prefix with 'scene'. Code here is VERY replaceable and I don't care as much what's going on in there from an engine developer point of view

* Momo - The part that is 'platform agnostic', which I will prefix with my personal alias 'momo'. This is equivalent of my 'std' library.

* moe_t - The part that is the meat of the engine code that runs 'scenes'. This is prefixed with 'moe' which stands for 'My Own Engine'.

Before, 'scene' and 'engine' was one thing, which works, but it would be nice to identify what are game-specific code and what are engine-specific code so that I can have the right mindset when I'm working with either.

The first thing I identified to change is the state management. Right now, what scenes there are is right in the core of the engine, which is definitely wrong with this mentality going forward. We will have to figure out how to move the 'knowledge of what scenes there are' away from the engine layer.


