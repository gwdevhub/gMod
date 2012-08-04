Warnung: Du verwendest diese Programm auf eigene Gefahr hin!
1) Wenn das Programm abstürzt, wirst du wahrscheinlich keinen Support
    vom Spielehersteller bekommen. Aber du kannst den Fehler gerne
    angeben unter:  http://code.google.com/p/texmod/issues/list
2) Spiele können detektieren, ob sie verändert werden, daher riskierst du
    einen Bann deines online Accounts.
3) Dies ist eine open-source Projekt. Der Code kann von jedem erhalten, verändert
    und kompiliert werden. Lade Universal Modding Engine nur von offiziellen Quellen runter,
    denen du vertraust. Lade es selbst runter und verwende keine Versionen,
    die du von Team- oder Gildenmitgliedern geschickt bekommen hast.
    http://code.google.com/p/texmod/downloads/list

   
Universal Modding Engine verwendet die D3DX9_43.dll (32bit). Wegen den EULA kann
diese dll nicht mit Universal Modding Engine mit geliefert werden. Wenn diese dll auf
deinem System nicht installiert ist, wird dich Universal Modding Engine darauf hinweisen.


Was kann Universal Modding Engine (uMod) V2.0?

-einzelne Texturen aus einem Spiel extrahieren und speichern (Die Zieltextur kann im Spiel geändert werden.)
-alle Texturen aus einem Spiel extrahieren und speichern (mit Größen- und Texturformat-Filter)
-Texturen in ein Spiel laden und Zieltexturen ersetzen
-Support einzelner Texturen
-Support von zip-Dateien (mit Darstellung von Textur-Gruppen als Baumstruktur)
-Support der originalen TexMod *.tpf Dateien (der Support muss über das Kontext-Menü aktiviert werden)

Alle diese Optionen können während des Spieles an und aus geschaltet werden!
Du kannst also nach einer Textur suchen, diese speichern, sie anschließend
editieren, sie in das Spiel laden, sie nach bearbeiten und wieder in das
Spiel laden, ... oder Mods aktivieren und deaktivieren
und das alles ohne das Spiel neu starten zu müssen.

Randbemerkung: Wenn alle Texturen gespeichert werden sollen, so geschieht das nur,
wenn die Texturen vom Spiel geladen werden und auch nur in dem Moment, wenn sie
geladen werden. Stellst du diese Option an, während die Map bereits geladen ist, wird
wahrscheinlich nichts geschehen, da alle Texturen für diese Map bereits geladen sind.
Wechsle also die Map oder lade erneut.

Zip Dateien können eine "content.txt" Datei enthalten, welche Informationen Gruppen und Texturen enthält.
Siehe dazu die zwei mitgelieferten Beispiele.

Wenn du einzelne Dateien lädst, sollten diese dem Wildcard  "*_hash.dds" entsprechen.


Wie interagiert Universal Modding Engine mit den Spielen?

Universal Modding Engine klingt sich zwischen die Verbindung vom Spiel und DirectX ein.
Das verlangt, dass Universal Modding Engine bereits vor dem Spiel gestartet wird.

Die GUI von Universal Modding Engine fungiert als Server. Ein Spiel, in das sich erfolgreich
eingeklingt wurde, verbindet sich beim Programmstart mit dem Server.
Die GUI von Universal Modding Engine kann mehrere Spiele gleichzeitig verwalten.
Jede Instanz eines Spieles wird auch separat gehandhabt (wenn ein Spiel
mehrfach gestartet wird).


Wie bekomme ich Universal Modding Engine zum laufen?

Es gibt drei Wege wie uMod sich in die DirectX Verbindung einklinken kann:
(Benutze NICHT mehrere Methoden gleichzeitig!)

1) Füge die Spiel-exe über das Menü "Einstellungen->Spiel hinzufügen" hinzu.
    Für Steam siehe unten.
    
    bekannte Probleme: Guild Wars (Win XP)
    
2) Starte das Spiel direkt durch uMod über das Menü
     "Einstellungen->Sarte Spiel durch uMod" oder
     "Einstellungen->Sarte Spiel durch uMod (mit Kommandozeile)".
     Das Spiel startet sofort.

3) Kopiere die d3d9.dll (vom uMod Verzeichnis) in das Spiele Verzeichnis.
    Einige Spiele laden eine dll zuerst aus dem eigenen Verzeichnis bevor sie im
    Systemverzeichnis suchen. Nur für diese Spiele wird diese Methode funktionieren.
    WARNUNG: Kopiere diese dll niemals in das Systemverzeichnis!!
    
    bekannte Probleme: Guild Wars
   
Wenn  du dich für die erste oder dritte Methode entschieden hast, starte einfach
Universal Modding Engine und danach das Spiel. Starte das Spiel in beiden Fällen NICHT über
Universal Modding Engine .

Wenn das Spiel startet und alles glatt läuft, öffnet sich sofort ein neuer Tab in uMod.
Im obersten Feld steht die Methode, welche verwendet wurde um sich in das Spiel
ein zu klinken. Danach sollten Informationen über erzeugte DirectX Devices stehen.
z.B. (DX9) oder (DX9EX: 3). Dies gibt an, welches Device und wie viele durch das Spiel
gestartet (und von uMod umgeleitet) wurden. Fehlt diese Information, so ist das Modden
nicht möglich. Mögliche Ursachen können sein, dass das Spiel kein DirectX 9 verwendet
oder uMod zu langsam war, um sich in das Spiel ein zu klinken.


Wenn Du Veränderungen vornimmst, so musst Du diese über das Kontext-Menü
(rechte Maustaste im unteren Fenster) -> Update bestätigen. 
Update (neu laden) bewirkt, dass die Texturen neu von der Festplatte geladen werden.
Diese Option ist nur für diejenigen sinnvoll, die Texturen editieren.

Neue Mods oder Texturen kannst Du über das Kontext-Menü oder per Drag&Drop aus
dem Explorer heraus öffnen. Die Reihenfolge der Mods veränderst Du ebenfalls über
Drag&Drop. Die Reihenfolge kann dann wichtig sein, wenn zwei Mods, die gleiche 
Spiele-Textur verändern wollen. Dann wird die Änderung des Mods aktiv, welcher weiter
oben gelistet ist.

Zusätzlich kannst Du jeden Mod aufklappen und einzelne Texturen aktivieren oder deaktivieren.
Mods mit dem neuen Mod-Format können auch mehrere Ebenen von Gruppen enthalten.
Hier kannst du einzelne Gruppen aktivieren und deaktivieren.
Während bei Texturen immer eines der beiden Häkchen gesetzt sein muss, können beim
Mod-Paket oder bei Gruppen beide Häkchen entfernt werden, um die Kontrolle an die nächst
tiefer liegende Ebene zu übergeben.

Wie bekommt man Universal Modding Engine mit Steam zum Laufen?

uMod schaut auf den Namen und auf das Verzeichnis der ausgeführten Spiel-exe.
Daher solltest du nicht die steam.exe sondern die Spiel.exe hinzufügen.
z.B.: C:\Steam\SteamApps\acoount_name\portal\hl2.exe
Wenn du die entsprechende exe nicht findest, kannst du das Spiel normal starten und 
über den TaskManager die entsprechende exe suchen. Über Rechstklick->Eigenschaften
erfährst du dann auch den Pfad der exe.