Icons fuer Flutter: transparenter Hintergrund
=============================================

Flutter verwendet von diesen Icons nur den Alphakanal (die Farbe setzt das
Widget). Weisser Hintergrund muss daher transparent sein. Das Skript
make_transparent.py wandelt schwarz-auf-weiss Icons entsprechend um:

  weiss  -> voll transparent
  schwarz -> voll deckend
  grau   -> teilweise transparent (Kanten bleiben geglaettet)

Die Farbkanaele werden auf Schwarz gesetzt, die Dateien werden direkt
ueberschrieben. Bilder, die schon einen Alphakanal haben, werden
uebersprungen (ausser mit --force).

Voraussetzung: Python 3 mit Pillow  (pip install pillow)


Aufruf in einer Batch-Datei (.bat) bzw. in der Eingabeaufforderung
------------------------------------------------------------------

Ein einzelnes Icon:

  python make_transparent.py ayf-neues-icon.png

Mehrere Icons:

  python make_transparent.py ayf-a.png ayf-b.png ayf-c.png

Alle PNGs im Verzeichnis (bereits transparente werden uebersprungen):

  python make_transparent.py .

Aus einem beliebigen Verzeichnis heraus, Beispiel-Batch:

  @echo off
  set ICONDIR=%~dp0
  python "%ICONDIR%make_transparent.py" "%ICONDIR%"

  (%~dp0 ist das Verzeichnis, in dem die .bat-Datei liegt; die Batch-Datei
  also neben make_transparent.py ablegen.)

Ein Icon erneut umwandeln, obwohl es schon einen Alphakanal hat:

  python make_transparent.py --force ayf-brender.png

Hinweis: Die Umwandlung ist nicht umkehrbar. Vorher committen oder das
Original sichern; zuruecksetzen geht mit  git checkout -- <datei>.png
