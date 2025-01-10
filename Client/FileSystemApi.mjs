import path from 'node:path';
import { fileURLToPath } from 'url';
import { tmpdir } from 'os';

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

function createTempCopy(filePath) {
    let tempPath = tmpdir();

    if(!tempPath.endsWith(path.sep))
        tempPath += path.sep;

    console.log(`temp dir = ${tempPath}`);
}

export {splitPath, fileNameWithExtension, createTempCopy};