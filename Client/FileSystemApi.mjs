import path from 'node:path';
import fs from 'node:fs/promises';
import { fileURLToPath } from 'url';
import { tmpdir } from 'os';
import { randomUUID } from 'crypto';
import { shell } from 'electron';

// https://iamwebwiz.medium.com/how-to-fix-dirname-is-not-defined-in-es-module-scope-34d94a86694d
// https://byby.dev/node-dirname-not-defined
// https://nodejs.org/api/esm.html#importmeta
// https://gist.github.com/sindresorhus/a39789f98801d908bbc7ff3ecc99d99c
const __filename = fileURLToPath(import.meta.url); // get the resolved path to the file
const __dirname = path.dirname(__filename); // get the name of the directory

function splitPath(givenPath) {
  return givenPath.split(path.sep);
}

function fileNameWithExtension(givenPath) {
  return path.basename(givenPath);
}

// TODO: add file existence check by filePath.
// TODO: Maybe move file copying part to separate function.
async function previewFile(filePath, fileExtension) {
  let tempPath = tmpdir();

  if(!tempPath.endsWith(path.sep))
      tempPath += path.sep;

  const name = randomUUID().replace(/-/g, ''); // Generate UUID and remove dashes
  let tempFilePath = path.join(tempPath, name);

  if(fileExtension && fileExtension !== '')
  {
    if(!fileExtension.startsWith('.'))
      fileExtension = `.${fileExtension}`;

    tempFilePath += fileExtension;
  }

  try {
    await fs.copyFile(filePath, tempFilePath);
    await shell.openPath(tempFilePath); // TODO: Add temp file cleaning.
    return true;
  } catch(error) {
    console.error(`Error previewing file from ${filePath} to ${tempFilePath}:`, error);
    return false;
  }
}

// TODO: Maybe move file copying part to separate function.
async function extractFile(srcPath, destPath) {
  try {
    await fs.copyFile(srcPath, destPath);
    shell.showItemInFolder(destPath);
    return true;
  } catch (error) {
    console.error(`Error extracting file from ${srcPath} to ${destPath}:`, error);
    return false;
  }
}

export {splitPath, fileNameWithExtension, previewFile, extractFile};