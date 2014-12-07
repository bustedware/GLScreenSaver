GLScreenSaver
=============
The Following Is For Micrsoft Visual Studio
(This can also be done in other IDE's)

To name your screen saver, you need to add a string table resource to your project.
Then you need to type in IDS_DESCRIPTION=1, hit enter, then type in the name of 
your scr under the caption.

To associate your screensaver with an icon, you need to add an icon resource
Once you have imported or created an icon, set its ID to ID_APP.

To create a settings dialog for when a user wants to change various settings for your
scr, you need to add a dialog resource. Once you have finished your dialog, set its ID
to DLG_SCRNSAVECONFIGURE=2003

You will want to override 
void screen_saver_main();
in your main.cpp as it is the entry point for the screensaver binary. 
This is the callback method that is executed from GLScreenSaver that 
runs your OpenGL code.
