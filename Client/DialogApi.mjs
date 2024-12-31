import { dialog, app } from "electron";
import { fileURLToPath } from "url";
import fs from "node:fs";
import path from "path";

// https://iamwebwiz.medium.com/how-to-fix-dirname-is-not-defined-in-es-module-scope-34d94a86694d
// https://byby.dev/node-dirname-not-defined
// https://nodejs.org/api/esm.html#importmeta
// https://gist.github.com/sindresorhus/a39789f98801d908bbc7ff3ecc99d99c
const __filename = fileURLToPath(import.meta.url); // get the resolved path to the file
const __dirname = path.dirname(__filename); // get the name of the directory

async function showFolderSelectDialog () {
    const { canceled, filePaths } = await dialog.showOpenDialog({ properties: ['openDirectory'] });
    let result = null;

    if (!canceled) {
        let stack = [{
            folderPath: filePaths[0],
            childFiles: [],
            childFolders: []
        }];

        let result = stack[0];
    
        while (stack.length > 0) {
            const currentFolder = stack.pop();
    
            const files = await fs.promises.readdir(currentFolder.folderPath);
    
            files.forEach(file => {
                const fullPath = path.join(currentFolder.folderPath, file);
                const stats = fs.statSync(fullPath);
    
                if (stats.isDirectory()) {
                    const folderObj = {
                        folderPath: fullPath,
                        childFiles: [],
                        childFolders: []
                    };
                    currentFolder.childFolders.push(folderObj);
                    stack.push(folderObj);
                } else {
                    currentFolder.childFiles.push(fullPath);
                }
            });
        }

        return result;
    }
}

async function showFileSelectDialog () {
  const { canceled, filePaths } = await dialog.showOpenDialog({ properties: ["openFile"], defaultPath: app.getPath("desktop") });
  let result = null;

  if (!canceled)
    result = filePaths[0];

  return result;
}

async function showFileSaveDialog () {
  const { canceled, filePath } = await dialog.showSaveDialog({ defaultPath: app.getPath('desktop') });
  let result = null;

  if (!canceled)
    result = filePath;

  return result;
}

export {showFolderSelectDialog, showFileSelectDialog, showFileSaveDialog};