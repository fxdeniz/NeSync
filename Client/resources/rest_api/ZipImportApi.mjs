import {fetchJSON, postJSON, BaseApi} from "./BaseApi.mjs";

export default class ZipExportApi extends BaseApi {

    constructor(hostName, port) {
        super(hostName, port);
    }

    async getFilePath() {
        return await fetchJSON(`http://${this.host}:${this.port}/import/zip/getFilePath`);
    }

    async setFilePath(filePath) {
        let requestBody = {"filePath": null};
        requestBody["filePath"] = filePath;
        
        return await postJSON(`http://${this.host}:${this.port}/import/zip/setFilePath`, requestBody);
    }

    async openZip() {
        return await fetchJSON(`http://${this.host}:${this.port}/import/zip/open`);
    }

    async readFoldersJson() {
        return await fetchJSON(`http://${this.host}:${this.port}/import/zip/readFoldersJson`);
    }

    async readFilesJson() {
        return await fetchJSON(`http://${this.host}:${this.port}/import/zip/readFilesJson`);
    }

    async importFileFromZip(symbolFilePath, versionNumber) {
        let requestBody = {"symbolFilePath": null, "versionNumber": null};
        requestBody["symbolFilePath"] = symbolFilePath;
        requestBody["versionNumber"] = versionNumber;
        
        return await postJSON(`http://${this.host}:${this.port}/import/zip/importFileFromZip`, requestBody);    
    }
}