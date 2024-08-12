# Brainstorm 🧠

I have both [Calibre](https://github.com/kovidgoyal/calibre) and the [Kobo Extended Driver](https://github.com/jgoguen/calibre-kobo-driver) open right now. I need to work off main Calibre repo because I currently use the Kepub conversion (AKA "Send books as kepubs" checkbox) on the Extended tab, which inherits from KoboTouch config.

 From what I can see in the code, as a Python amateur and infrequent user, the Extended driver inherits the KoboTouch driver, a built-in plug-in in Calibre (so many "in"s). This is my first time dealing with Python development and as a result, inheritance.

# Tracing Code: In progress :)

## Calibre: KoboTouch Plug-in

### kobotouch_config.py file

This is where I might add my widget.

### driver.py file (the main part of the logic)
