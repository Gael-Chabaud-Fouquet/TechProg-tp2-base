# Base pour travail pratique du cours Technique de programmation

Faite vous une version du repository à l'aide du "Use template" de github.

Lorsque vous avez votre version du repository, faite la commande suivante:
```cmd
git clone --recursive https://github.com/MON/REPO/AMOI.git
```

Remplacer `MON/REPO/AMOI` par le chemin de votre repository.

Assurer vous que `kmake.exe` existes dans le dossier, `Tools\windows_x64`. Si jamais ce n'est pas le cas veuillez faire la commande:
```cmd
git submodule update --init
```

## Build

Le fichier `kfile.js` est la définition de votre solution, il contient la ligne `const project = new Project('monprojet');` et il est **OBLIGATOIRE** de renommer le projet, donc changer le texte `'monprojet'` pour le nom de votre projet. Cela nommeras votre solution.

Pour la génération de la solution Visual Studio, vous devrez utiliser kmake qui se trouve dans le dossier `Tools\windows_x64\kmake.exe`.

Faite la commande suivante à partir du dossier source qui contient le kfile.js soit `TechProg-tp-base` ou le nom que vous avez donné à votre repository:
```cmd
.\Tools\windows_x64\kmake.exe
```

Si jamais vous êtes dans vscode il est aussi possible de pèser sur `ctrl+shift+b` pour faire la commande si haut.

### LISEZ LE KFILE
Pour bien comprendre le générateur de solution veuillez lire le fichier `kfile.js` qui contient des commentaires pour vous aider à mieux le comprendre.

### IMPORTANT

Vous devrez générer la solution à chaque fois que vous voulez ajouter un nouveau fichier au projet, vous ne pourrez pas créer les fichiers par l'interface de Visual Studio, entre autres pacre que `the interface is a lie`. Une bonne pratique est d'avoir vscode d'ouvert pour facilement regénérer la solution.