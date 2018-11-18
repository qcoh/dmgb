# Cycle accuracy

As I currently understand, JIT compilation as cycle accuracy are at odds with
each other: If I compile, say, 100 GameBoy instructions to some 100+ x86\_64
instructions and execute this block, I might miss the next VBlank, Timer or
Joypad interrupt.

Whenever I expect and interrupt, I could remain in interpreter mode however
the Joypad interrupt will still have to wait.

I recently thought about the cycle times about more and that an instruction
might decompose into

	[Fetch from memory][Execute][Store in memory]

where each takes four cycles.

Now, the idea for fine-grained interrupts while executing a compiled block
is to do two things:

* Whenever an interrupt becomes enabled, protect the entire memory from
reading and writing and in the next segmentation fault, disable the JIT and
handle the interrupt while in interpreter mode
* Have a page for the registers, i.e. all registers are stored in memory,
not as host registers
