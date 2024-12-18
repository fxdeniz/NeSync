import {fetchJSON, postJSON} from "./BaseApi.mjs";

export default class FolderApi {
    #host;
    #port;

    constructor(hostName, port) {
        this.#host = hostName;
        this.#port = port;
    }

    async getFolderContent(symbolFolderPath) {
        let requestBody = {"symbolPath": null};
        requestBody["symbolPath"] = symbolFolderPath;

        return await postJSON(`http://${this.#host}:${this.#port}/getFolderContent`, requestBody);
    }

    async getFolderByUserPath(userFolderPath) {
      let requestBody = {"userFolderPath": null};
      requestBody["userFolderPath"] = userFolderPath;

      return await postJSON(`http://${this.#host}:${this.#port}/getFolderContentByUserPath`, requestBody);
    }

    async addFolder(symbolFolderPath, userFolderPath) {
      let requestBody = {"symbolFolderPath": null, "userFolderPath": null};
      requestBody["symbolFolderPath"] = symbolFolderPath;
      requestBody["userFolderPath"] = userFolderPath;
    
      return await postJSON(`http://${this.#host}:${this.#port}/addNewFolder`, requestBody);    
    }

    async deleteFolder(symbolFolderPath) {
      return await fetchJSON(`http://${this.#host}:${this.#port}/deleteFolder?symbolPath=${symbolFolderPath}`, "DELETE");
    }
}