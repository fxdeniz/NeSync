import {fetchJSON, postJSON} from "./BaseApi.mjs";

export default class ZipExportApi {
    #host;
    #port;

    constructor(hostName, port) {
        this.#host = hostName;
        this.#port = port;
    }

    async setImportZipFilePath(filePath) {
        let requestBody = {"filePath": null};
        requestBody["filePath"] = filePath;
        
        return await postJSON(`http://${this.#host}:${this.#port}/zip/import/ZipFilePath`, requestBody);
    }

    async openImportZip() {
        return await fetchJSON(`http://${this.#host}:${this.#port}/zip/import/OpenFile`);
    }

    async readFoldersJson() {
        return await fetchJSON(`http://${this.#host}:${this.#port}/zip/import/ReadFoldersJson`);
    }

    async readFilesJson() {
        return await fetchJSON(`http://${this.#host}:${this.#port}/zip/import/ReadFilesJson`);
    }

    async importFile(symbolFilePath, versionNumber) {
        let requestBody = {"symbolFilePath": null, "versionNumber": null};
        requestBody["symbolFilePath"] = symbolFilePath;
        requestBody["versionNumber"] = versionNumber;
        
        return await postJSON(`http://${this.#host}:${this.#port}/zip/import/file`, requestBody);    
    }
}