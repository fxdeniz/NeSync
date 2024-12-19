import {fetchJSON, postJSON, BaseApi} from "./BaseApi.mjs";

export default class FileApi extends BaseApi {

    constructor(hostName, port) {
        super(hostName, port);
    }

    async get(symbolFilePath) {
      let requestBody = {};
      requestBody["symbolPath"] = symbolFilePath;
      
      return await postJSON(`http://${this.host}:${this.port}/file/get`, requestBody);
    }

    async getByUserPath(userFilePath) {
      let requestBody = {};
      requestBody["userFilePath"] = userFilePath;

      return await postJSON(`http://${this.host}:${this.port}/file/getByUserPath`, requestBody);
    }

    async add(symbolFolderPath, pathToFile, description, isFrozen) {
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

    async delete(symbolFilePath) {
      return await fetchJSON(`http://${this.host}:${this.port}/file/delete?symbolPath=${symbolFilePath}`, "DELETE");
    }
}