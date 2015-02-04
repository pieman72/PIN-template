Installation and Testing
===

Linux
---
1. Go to the [PIN download page](https://software.intel.com/en-us/articles/pintool-downloads).

2. Download the most recent version for Linux ("Sep 02, 2014", when I looked)

3. Unpack the file, rename it to "PIN", and move it into `/path/to/`, for
convenience:
	$ tar -xf pin-2.14-67254-gcc.4.4.7-linux.tar.gz
	$ mv pin-2.14-67254-gcc.4.4.7-linux /path/to/PIN

**At this point, the executable, `/path/to/PIN/pin`, is ready to use. The
following instructions are just for testing your setup.**

4. Enter the PIN Examples directory:
	$ cd /path/to/PIN/source/tools/ManualExamples

5. Compile all of the existing PIN tools with `make`:
	$ make

6. Try to run then PIN instruction-count trace on a simple binary like `ls`:
	$ ../../../pin -t obj-ia32/inscount0.so -- /bin/ls

7. If it tells you "`Attach to pid xxxxx failed`", you may need to enable
arbitrary pthread tracing (arguably a security concern), by doing:
	$ echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope
Then try step #6 again

8. If it tells you "`E:Unable to load obj-ia32/inscount0.so`", then you may need
to use the instruction-count tool compiled for a different architecture:
	$ ../../../pin -t obj-intel64/inscount0.so -- /bin/ls

9. Once PIN traces the `ls` execution successfully, in addition to the expected
`ls` output to the console, the tool will create a file called `inscount.out`
containing a readout of the number of machine instructions performed:
	$ cat inscount.out
	Count 833535

10. If everything is working correctly, you may wish to make an alias for PIN:
	$ alias pin='/path/to/PIN/pin'



Setup for Creating Your Own PIN Tools
===
1. Start at the PIN root directory:
	$ cd /path/to/PIN/

2. Create a new directory for your tool, and move to it
	$ mkdir source/tools/SOME_TOOL
	$ cd source/tools/SOME_TOOL

3. Copy the source files for the existing `MyPinTool`:
	$ cp -R ../MyPinTool/* .

4. Rename the cpp file, and change `MyPinTool` to `SOME_TOOL` in
`makefile.rules`:
	$ mv MyPinTool.cpp SOME_TOOL.cpp
	$ sed -i 's/MyPinTool/SOME_TOOL/g' makefile.rules

5. Now you are ready to start building your tool. You can edit the existing
contents of `SOME_TOOL.cpp` and add your own content. The API reference is
available [here](https://software.intel.com/sites/landingpage/pintool/docs/71313/Pin/html/group__API__REF.html).

6. Compile your tool by running `make` in the `SOME_TOOL` directory:
	$ make

7. You can now make an alias for your new tool:
	$ alias SOME_TOOL='/path/to/PIN/pin -t /path/to/PIN/source/tools/SOME_TOOL/obj-ia32/SOME_TOOL.so -- '

8. Using the alias, you can now run your tool very easily:
	$ SOME_TOOL /bin/ls
