import {fetchJSON, postJSON} from "./BaseApi.mjs";

export default class FileApi {
    #host;
    #port;

    constructor(hostName, port) {
        this.#host = hostName;
        this.#port = port;
    }

    async getFile(symbolFilePath) {
      return await fetchJSON(`http://${this.#host}:${this.#port}/getFileContent?symbolPath=${symbolFilePath}`);
    }

    async getFileByUserPath(userFilePath) {
      return await fetchJSON(`http://${this.#host}:${this.#port}/getFileContentByUserPath?userFilePath=${userFilePath}`);
    }

    async addFile(symbolFolderPath, pathToFile, description, isFrozen) {
      let requestBody = {};
      requestBody["symbolFolderPath"] = symbolFolderPath;
      requestBody["pathToFile"] = pathToFile;
      requestBody["description"] = description;
      requestBody["isFrozen"] = isFrozen;
    
      return await postJSON(`http://${this.#host}:${this.#port}/addNewFile`, requestBody);    
    }

    async appendVersion(pathToFile, description) {
      let requestBody = {};
      requestBody["pathToFile"] = pathToFile;
      requestBody["description"] = description;
    
      return await postJSON(`http://${this.#host}:${this.#port}/appendVersion`, requestBody);    
    }

    async deleteFile(symbolFilePath) {
      return await fetchJSON(`http://${this.#host}:${this.#port}/deleteFile?symbolPath=${symbolFilePath}`, "DELETE");
    }
}