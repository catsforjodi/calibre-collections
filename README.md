# Proposed Additions to the [KoboTouch Driver in the Calibre Repo](https://github.com/kovidgoyal/calibre.git)
(This is my first time and experience with OSS. Any suggestions are welcome!)

## Premise
I currently use Kobo Clara HD E-reader and through hours of reading and admiring the helpfulness on Reddit and MobileRead Forum, I downloaded Calibre. I use jgoguen's [Kobo Touch Extended Plug-in](https://github.com/jgoguen/calibre-kobo-driver) to convert my sideloaded epubs on my PC to kepubs on my Kobo. Kepubs make turning the pages on my Kobo so much faster, and they also enable Kobo to show built-in reading statistics. Using [this](https://www.reddit.com/r/kobo/comments/qlgha1/how_to_create_kobo_collections_using_calibre_basic/) informative thread on Reddit, I was able to set the Calibre to turn all the unique book tags into collections. However, due to the large number of unique tags (40+) in my libary, I found it difficult to scroll through the Collections tab in my Kobo to get to my favourite tags. 

## Proposed Solution

I want my Kobo Collections tab only to show books associated to tags I want (e.g. Murder Mystery, Historical Romance, Non-Fiction). Right now I have set the Collections Template in davidfor's Kobo Touch to filter through the tags, only returning the ones I wanted in the Collections:
```
program:
	for tag in field('tags'):
		if tag == 'Historical Romance' then return 'Historical Romance'
		fi
rof
```

It's just a `for` loop. Nothing special, but it took me a few hours to figure out what the template even did and how it worked. I realize now I could have totally just returned `tag` instead of typing `'Historical Romance'` twice. Thanks to a helpful [MobileRead user](https://www.mobileread.com/forums/showpost.php?p=4412871&postcount=2962), I had a direction to go off of and they gave me multiple options! 

Note: In Calibre, I have to mass add `Historical Romance` tag to every book I want to show under that tag in Collections, so that this `for` loop could filter out the books. That's pretty easy to do in Calibre. Just a lot of Shift-Clicks.

__Now__ I want to add to the GUI a little textbox where one can list the tags they want to show up in Kobo Collections.

### Technologies I'll Probably Learn and Use
It would be super simple for me to add unique tags in this code if I wanted to, but I have decided to over engineer something so I could learn/apply skills:

- Python 
    - I have used it in the past for some Data Analysis, but never for development
- Qt 
    - Looking through the existing code, I'll have to use [QtWidgets](https://doc.qt.io/qtforpython-5/PySide2/QtWidgets/QGraphicsScene.html#PySide2.QtWidgets.PySide2.QtWidgets.QGraphicsScene.addWidget) to add a checkbox and textbox
- Open source development (in general)
    - this is my first time :0
    - plug-ins
- [Calibre Template Language](https://manual.calibre-ebook.com/template_lang.html)
    - this is what the language the above `for` loop is written in. 
