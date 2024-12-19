import {fetchJSON, postJSON, BaseApi} from "./BaseApi.mjs";

export default class FileApi extends BaseApi {

    constructor(hostName, port) {
        super(hostName, port);
    }

    async getFile(symbolFilePath) {
      let requestBody = {};
      requestBody["symbolPath"] = symbolFilePath;
      
      return await postJSON(`http://${this.host}:${this.port}/file/get`, requestBody);
    }

    async getFileByUserPath(userFilePath) {
      let requestBody = {};
      requestBody["userFilePath"] = userFilePath;

      return await postJSON(`http://${this.host}:${this.port}/file/getByUserPath`, requestBody);
    }

    async addFile(symbolFolderPath, pathToFile, description, isFrozen) {
      let requestBody = {};
      requestBody["symbolFolderPath"] = symbolFolderPath;
      requestBody["pathToFile"] = pathToFile;
      requestBody["description"] = description;
      requestBody["isFrozen"] = isFrozen;
    
      return await postJSON(`http://${this.host}:${this.port}/file/add`, requestBody);    
    }

    async appendVersion(pathToFile, description) {
      let requestBody = {};
      requestBody["pathToFile"] = pathToFile;
      requestBody["description"] = description;
    
      return await postJSON(`http://${this.host}:${this.port}/file/append`, requestBody);    
    }

    async deleteFile(symbolFilePath) {
      return await fetchJSON(`http://${this.host}:${this.port}/file/delete?symbolPath=${symbolFilePath}`, "DELETE");
    }
}