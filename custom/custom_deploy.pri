QMAKE_POST_LINK += && $$QMAKE_COPY $$PWD/deploy/Lince-GroundControl-start.sh $$DESTDIR
QMAKE_POST_LINK += && $$QMAKE_COPY $$PWD/deploy/Lince-GroundControl.desktop $$DESTDIR
QMAKE_POST_LINK += && $$QMAKE_COPY $$PWD/deploy/linuxIcon.png $$DESTDIR