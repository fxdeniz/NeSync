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

    async add(symbolFolderPath, userFolderPath) {
      let requestBody = {"symbolFolderPath": null, "userFolderPath": null};
      requestBody["symbolFolderPath"] = symbolFolderPath;
      requestBody["userFolderPath"] = userFolderPath;
    
      return await postJSON(`http://${this.host}:${this.port}/folder/add`, requestBody);    
    }

    async delete(symbolFolderPath) {
      let requestBody = {};
      requestBody["symbolPath"] = symbolFolderPath;

      return await postJSON(`http://${this.host}:${this.port}/folder/delete`, requestBody, "DELETE");
    }
}