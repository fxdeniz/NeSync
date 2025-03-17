import {postJSON, BaseApi} from "./BaseApi.mjs";

export default class FileApi extends BaseApi {

    constructor(hostName, port) {
        super(hostName, port);
    }

    async updateDescription(symbolFilePath, versionNumber, description) {
      let requestBody = {};
      requestBody["symbolFilePath"] = symbolFilePath;
      requestBody["versionNumber"] = versionNumber;
      requestBody["description"] = description;
      
      return await postJSON(`http://${this.host}:${this.port}/version/update`, requestBody);
    }
}