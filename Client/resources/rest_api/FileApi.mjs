import {fetchJSON, postJSON, BaseApi} from "./BaseApi.mjs";

export default class FileApi extends BaseApi {

    constructor(hostName, port) {
        super(hostName, port);
    }

    async getFile(symbolFilePath) {
      let requestBody = {};
      requestBody["symbolPath"] = symbolFilePath;
      
      return await postJSON(`http://${this.host}:${this.port}/getFileContent`, requestBody);
    }

    async getFileByUserPath(userFilePath) {
      let requestBody = {};
      requestBody["userFilePath"] = userFilePath;

      return await postJSON(`http://${this.host}:${this.port}/getFileContentByUserPath`, requestBody);
    }

    async addFile(symbolFolderPath, pathToFile, description, isFrozen) {
      let requestBody = {};
      requestBody["symbolFolderPath"] = symbolFolderPath;
      requestBody["pathToFile"] = pathToFile;
      requestBody["description"] = description;
      requestBody["isFrozen"] = isFrozen;
    
      return await postJSON(`http://${this.host}:${this.port}/addNewFile`, requestBody);    
    }

    async appendVersion(pathToFile, description) {
      let requestBody = {};
      requestBody["pathToFile"] = pathToFile;
      requestBody["description"] = description;
    
      return await postJSON(`http://${this.host}:${this.port}/appendVersion`, requestBody);    
    }

    async deleteFile(symbolFilePath) {
      return await fetchJSON(`http://${this.host}:${this.port}/deleteFile?symbolPath=${symbolFilePath}`, "DELETE");
    }
}