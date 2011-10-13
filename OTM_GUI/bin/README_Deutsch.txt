Warnung: Du verwendest diese Programm auf eigene Gefahr hin!
1) Wenn das Programm abstürzt wird du wahrscheinlich keinen Support
    vom Spielehersteller bekommen. Aber du kannst den Fehler gerne
    angeben unter  http://code.google.com/p/texmod/issues/list
2) Spiele können detektieren ob sie verändert werde, daher riskierst du
    einen Bann deines online Accounts.
3) Dies ist eine open-source Projekt. Der Code kann von jedem erhalten, verändert
    und kompiliert werden. Lade OpenTexMod nur von offiziellen Quellen runter,
    denen du vertraust. Lade es selbst runter und verwende keine Versionen
    die du von Team- oder Gildenmitgliedern geschickt bekommen hast.
    http://code.google.com/p/texmod/downloads/list

   
OpenTexMod verwendet die D3DX9_43.dll. Wegen den EULA kann diese dll nicht mit OpenTexMod
mit geliefert werden. Wenn diese dll auf deinem System nicht installiert ist, wird dich OpenTexMod
darauf hinweisen.


Was kann OpenTexMod alpha V0.9?

-einzelne Texturen aus einem Spiel extrahieren und speichern (Die Zieltextur kann im Spiel geändert werden.)
-alle Texturen aus einem Spiel extrahieren und speichern
-Texturen in ein Spiel laden und Zieltexturen ersetzen
-Support von zip-Dateien
-Support der originalen TexMod *.tpf Datein

Alle diese Optionen können während des Spieles an und aus geschaltet werden!
Du kannst also nach einer Textur suchen, diese speichern, sie anschließend
editieren, sie in das Spiel laden, sie nach bearbeiten und wieder in das
Spiel laden, ...
und das alles ohne das Spiel neu starten zu müssen.

Randbemerkung: Wenn alle Texturen gespeichert werden sollen, so geschieht das nur,
wenn die Texturen vom Spiel geladen werden und auch nur in dem Moment, wenn sie
geladen werden. Stellst du diese Option an, während die Map bereits geladen ist, wird
wahrscheinlich nichts geschehen, da alle Texturen für diese Map bereits geladen sind.
Wechsle also die Map oder lade erneut.

Zip Dateien können eine "texmod.def" Datei enthalten, welche die Hash-Werte und
Dateinamen enthält. Jede Zeile sollte dem Format "hash|filename.dds" entsprechen.
Wenn es keine "texmod.def" Datei enthält, wird jede Datei entpackt und jene, deren 
Namen auf das Wildcard  "*_hash.dds" zutrifft, werden benutzt.
Die Zip Datei kann eine "Comment.txt" enthalten. Der Inhalte wird als Kommentar
in der GUI angezeigt.

Wenn du einzelne Dateien lädst, sollten diese dem Wildcard  "*_hash.dds" entsprechen.


Wie interagiert OpenTexMod mit den Spielen?

OpenTexMod klingt sich zwischen die Verbindung vom Spiel und DirectX ein.
Das verlangt, dass OpenTexMod bereits vor dem Spiel gestartet wird.

Die GUI von OpenTexMod fungiert als Server. Ein Spiel, in das sich erfolgreich
eingeklingt wurde, verbindet sich beim Programmstart mit dem Server.
Die GUI von OpenTexMod kann mehrere Spiele gleichzeitig verwalten.
Jede Instanz eines Spieles wird auch separat gehandhabt (wenn ein Spiel
mehrfach gestartet wird).


Wie bekomme ich OpenTexMod zum laufen?

Es gibt zwei Wege wie OpenTexMod sich in die DirectX Verbindung einklinken kann:

1) (bevorzugt)  Füge die Spiel-exe über das Menü Spiele->"Spiel hinzufügen" hinzu.
    Für Steam siehe unten.
   
2) Kopiere die d3d9.dll (vom OpenTexMod Verzeichnis) in das Spiele Verzeichnis.
    Einige Spiele laden eine dll zuerst aus dem eigenen Verzeichnis bevor sie im
    Systemverzeichnis suchen. Nur für diese Spiele wird diese Methode funktionieren.
    WARNUNG: Kopiere diese dll niemals in das Systemverzeichnis!!
   
Wenn  du dich für eine Methode entschieden hast, starte einfach OpenTexMod
und danach das Spiel. Es ist nicht nötig (du kannst auch nicht) das Spiel über
OpenTexMod zu starten.

Wenn das Spiel startet und alles glatt läuft, öffnet sich sofort ein neuer Tab in OpenTexMod.
In diesem Tab kannst du nun das Spiel  modden. Drücke den "Update" Button um
die Einstellungen an das Spiel zu senden. Du kannst deine aktuellen Einstellungen auch
als ein Template speichern. Ein Template kann auch als Standard eingestellt werden.
Es wird dann automatisch geladen und an das Spiel gesendet, wenn
dieses Spiel das nächste Mal gestartet wird.

Um einen Mod zu laden, musst du das Häkchen vor den Namen setzen. Wenn du
den Mod entladen willst, entferne das Häkchen und klicke auf "Update".

Der Update Button lädt nur Veränderungen (wenn Pakete aus der Liste entfernt
wurden, du die Häkchen verändert hast oder sich die Reihenfolge geändert hat)
Der "neu laden" Button erzwingt das Neuladen von Festplatte (z.B. wenn du die
Texturen verändert hast).

Weil verschiedene Mods die gleiche Ziel-Textur verändern könnten, wird nur die
Mod-Textur des ersten Mods berücksichtigt. Die Aktion dieses Mods (also Laden oder
Entladen) wird durchgeführt, ungeachtet dessen, was die restlichen Mods mit der
Ziel-Textur machen sollen.


Wie bekommt man OpenTexMod mit Steam zum Laufen?

OpenTexMod schaut auf den Namen und auf das Verzeichnis der ausgeführten Spiel-exe.
Daher solltest du nicht die steam.exe sondern die Spiel.exe hinzufügen.
z.B.: C:\Steam\SteamApps\acoount_name\portal\hl2.exe