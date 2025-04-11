let project = new Project('monprojet');

project.kore = false;
project.cmd = true;

// Si jamais vous creer des dossiers avec plusieurs .h, vous devez les ajouter
// comme un directory include pour que la solution puisse les utiliser.
// La ligne 9 est un example:
project.addIncludeDir('src');

// Chaque dossier qui a des .c doit faire comme la ligne 13, si jamais il y a des dossiers dans src,
// il n'est pas nécessaire de faire cette ligne vu que ** dit d'ajouter les dossier récursivement:
project.addFile('src/**');

// Établit le dossier d'execution du projet
project.setDebugDir('Deployment');

const fs = require('fs');
// Si jamais vous avez besoins d'ouvrir des fichiers avec votre executable(fopen) pour votre projet
// copier les comme suit dans le dossier Deployment qui sera le dossier d'execution de votre projet:
fs.copyFileSync(process.cwd()+'/assets/test.txt','./Deployment/test.txt');

resolve(project);