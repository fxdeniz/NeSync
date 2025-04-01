import {fetchJSON, postJSON, BaseApi} from "./BaseApi.mjs";

export default class FolderApi extends BaseApi {

    constructor(hostName, port) {
      super(hostName, port);
    }

    async get(symbolFolderPath) {
        let requestBody = {"symbolPath": null};
        requestBody["symbolPath"] = symbolFolderPath;

        return await postJSON(`http://${this.host}:${this.port}/folder/get`, requestBody);
    }

    async getByUserPath(userFolderPath) {
      let requestBody = {"userFolderPath": null};
      requestBody["userFolderPath"] = userFolderPath;

      return await postJSON(`http://${this.host}:${this.port}/folder/getByUserPath`, requestBody);
    }

    async getStorageFolderPath() {
      const result =  await fetchJSON(`http://${this.host}:${this.port}/folder/storageFolderPath`);
      return result.storageFolderPath;
    }

    async add(symbolFolderPath, userFolderPath) {
      let requestBody = {"symbolFolderPath": null, "userFolderPath": null};
      requestBody["symbolFolderPath"] = symbolFolderPath;
      requestBody["userFolderPath"] = userFolderPath;
    
      return await postJSON(`http://${this.host}:${this.port}/folder/add`, requestBody);    
    }

    async rename(symbolFolderPath, folderName) {
      let requestBody = {};
      requestBody["symbolFolderPath"] = symbolFolderPath;
      requestBody["folderName"] = folderName;

      return await postJSON(`http://${this.host}:${this.port}/folder/rename`, requestBody);
    }

    async freeze(symbolFolderPath) {
      let requestBody = {};
      requestBody["symbolFolderPath"] = symbolFolderPath;

      return await postJSON(`http://${this.host}:${this.port}/folder/freeze`, requestBody);
    }

    async relocate(symbolFolderPath, userFolderPath) {
      let requestBody = {};
      requestBody["symbolFolderPath"] = symbolFolderPath;
      requestBody["userFolderPath"] = userFolderPath;

      return await postJSON(`http://${this.host}:${this.port}/folder/relocate`, requestBody);
    }

    async delete(symbolFolderPath) {
      let requestBody = {};
      requestBody["symbolPath"] = symbolFolderPath;

      return await postJSON(`http://${this.host}:${this.port}/folder/delete`, requestBody, "DELETE");
    }
}