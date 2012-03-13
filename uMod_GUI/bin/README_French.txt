ATTENTION : Vous utilisez ce programme à vous risque !
1) Si le programme plante, vous n'aurez probablement aucune aide des développeurs,
    mais vous pouvez envoyer le bogue sur http://code.google.com/p/texmod/issues/list
2) Les jeux peuvent détecter les modifications de uMod, vous prenez le risque de bannir
    votre compte en ligne !
3) C'est un projet open-source. Le code peut être récupéré, modifié et compilé
    par tout le monde. Téléchargez Universal Modding Engine seulement sur des sources sûres !
    Ne télécharger pas et n'utiliser pas d'autres versions modifiées de ce logiciel,
    par exemple des versions envoyées par votre équipe ou votre guilde !
    Le site officiel : http://code.google.com/p/texmod/downloads/list

Universal Modding Engine utilise le D3DX9_43.dll (32bit). En raison de la licence CLUF cette dll
ne peut pas être livrée avec Universal Modding Engine . Si vous n'avez pas D3DX9_43.dll 
d'installé sur votre système, Universal Modding Engine va vous afficher un message
au démarrage du programme.


Que fait Universal Modding Engine (uMod) V1.0 ?

-Extraire et sauvegarder des textures d'un jeu DX9 (la texture d'origine peut se modifier)
-Extraire et sauvegarder toutes les textures d'un jeu DX9
-Charger des textures dans un jeu pour remplacer les textures d'origines
-Supporte les textures DDS
-Supporte les fichiers compressés (ZIP) pour faire un MOD (modifier plusieurs textures à la fois)
-Supporte les fichiers de TexMod *.tpf

Toutes les options peuvent s'activer ou se désactiver, pendant que le jeu est lancé !
Ainsi, vous pouvez rechercher une texture dans le jeu, l'enregistrer sur disque, la modifier,
la charger dans le jeu, éditer une autre texture, la charger dans le jeu, etc...,
Activer ou désactiver des MODs sans aucun redémarrage du jeu.

Remarque : Si vous cochez "Sauvegarder toutes les textures", les textures qui seront enregistrées,
sont les textures chargées dans le jeu après le moment où vous avez coché la case.
Si vous activez cette option, pendant qu'une carte est chargée, vous n'allez probablement rien enregistrer,
parce que toutes les textures sont déjà chargées. Changez de Carte ou rechargez-là.

Les fichiers compressés peuvent inclure le fichier "texmod.def" qui contient le hash et le nom du fichier.
Chaque ligne doit avoir le format "hash|filename.dds"
Si elle ne contient pas un "texmod.def" chaque fichier sera décompressé et seuls les fichiers ayant
le format "*_hash.dds" seront utilisés.
Le fichier zip peut inclure un fichier "Comment.txt". Le contenu sera affiché comme commentaire
dans l'interface graphique.

Si vous chargez des textures seules, elles doivent correspondre au format "*_hash.dds"


Comment Universal Modding Engine interagit avec les jeux ?

Universal Modding Engine intercepte la connexion entre le jeu et DirectX.
Il est nécessaire, que Universal Modding Engine soit lancé avant le jeu.

L'interface graphique de Universal Modding Engine est comme un serveur.
Un jeu correctement intercepté par Universal Modding Engine-dll se connectera au démarrage sur le serveur.
L'interface graphique de Universal Modding Engine peut gérer plusieurs jeux en même temps. Il sera
également traiter chaque jeu en tant que processus indépendant (si le même jeu
a été lancé plus d'une fois).


Comment Universal Modding Engine fonctionne ?

Il y a trois façon pour que Universal Modding Engine intercepte la connexion à DirectX :
(Ne PAS utiliser plus d'une méthode à la fois !)

1) Ajouter l'exécutable du jeu dans le menu "Réglages->Ajouter un jeu"
    Pour Steam, voir plus bas.
    
    Problèmes connus : Guild Wars (Win XP)
    
2) Lancer le jeu directement via le menu uMod
    "Réglages->Lancer le jeu avec uMod" ou
	"Réglages->Lancer le jeu avec uMod (en ligne de commande)".
	Le jeu se lance directement.

3) Copier le d3d9.dll (depuis le répertoire de Universal Modding Engine) dans votre dossier de jeu.
    Certain jeu recherche les DLLs dans leur répertoire avant d'aller chercher les DLLs dans
	le dossier du système.
    Cette méthode ne fonctionne que sur certain jeu.
    ATTENTION : Ne jamais copier ce fichier DLL dans votre répertoire système !!!
    
    Problèmes connus : Guild Wars

Si vous avez choisi la première ou la troisième méthode, il vous suffit de lancer Universal Modding Engine
et de lancer le jeu séparément. Ne PAS lancer le jeu via uMod.

Si le jeu se lance et tout fonctionne, un nouveau onglet va s'ouvrir directement dans uMod.
Dans cet onglet vous pouvez modifier le jeu. Appuyer sur "Mettre à jour" 
pour envoyer les changements au jeu. Vous pouvez sauvegarder vos paramètres actuels comme thème.
Un thème peut être lancé par défaut au démarrage d'un jeu.

Pour charger une texture ou un mod, vous devez cocher la case du fichier. Si vous voulez décharger un mod,
il faut simplement décocher la case et cliquer sur "Mettre à jour". 

En cliquant sur "Mettre à jour", seules les textures chargées dans uMod seront misent à jour
(si une texture chargée est supprimée de la liste, cela va juste décocher la texture).
Cliquez sur "Mettre à jour (Recharger)" pour recharger les fichiers de texture sur le disque dur.
(Si vous venez de modifier une texture déjà chargée par exemple).

En raison du fait que plusieurs mods peuvent modifier la même texture, seuls le mod / la texture
du premier fichier de la liste est pris en compte pour la modification. L'action de ce mod
(c'est à dire le chargement ou le déchargement) est effectuée, indépendamment des autres mods.


Comment faire pour que Universal Modding Engine fonctionne avec Steam ?

Universal Modding Engine cherche le nom et le chemin du fichier binaire exécuté.
Ainsi vous ne devez pas ajouter Steam.exe mais plutôt le game.exe
exemple : C:\Steam\SteamApps\acoount_name\portal\hl2.exe
Si vous ne trouvez pas l'exécutable, il suffit de commencer le jeu et
d'utiliser "le Gestionnaire des tâches" pour rechercher dans quel dossier il se trouve en
faisant un clique droit sur le nom de l'exécutable "Propriétés"->"Emplacement".