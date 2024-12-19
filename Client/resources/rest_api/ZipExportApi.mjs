import {fetchJSON, postJSON, BaseApi} from "./BaseApi.mjs";

export default class ZipExportApi extends BaseApi {

    constructor(hostName, port) {
        super(hostName, port);
    }

    async setFilePath(filePath) {
        let requestBody = {"filePath": null};
        requestBody["filePath"] = filePath;
        
        return await postJSON(`http://${this.host}:${this.port}/export/zip/setFilePath`, requestBody);
    }

    async getFilePath() {
        return await fetchJSON(`http://${this.host}:${this.port}/export/zip/getFilePath`);    
    }

    async getRootFolder() {
        return await fetchJSON(`http://${this.host}:${this.port}/export/zip/getRootFolder`);    
    }

    async setRootFolder(symbolPath) {
        let requestBody = {"rootSymbolFolderPath": null};
        requestBody["rootSymbolFolderPath"] = symbolPath;
        
        return await postJSON(`http://${this.host}:${this.port}/export/zip/setRootFolder`, requestBody);    
    }

    async createZip(filePath) {
        let requestBody = {"filePath": null};
        requestBody["filePath"] = filePath;
        
        return await postJSON(`http://${this.host}:${this.port}/export/zip/create`, requestBody);    
    }

    async addFoldersJson() {
        let requestBody = {};
        return await postJSON(`http://${this.host}:${this.port}/export/zip/addFoldersJson`, requestBody);    
    }

    async addFilesJson() {
        let requestBody = {};
        return await postJSON(`http://${this.host}:${this.port}/export/zip/addFilesJson`, requestBody);    
    }

    async addFile(symbolFilePath, versionNumber) {
        let requestBody = {"symbolFilePath": null, "versionNumber": null};
        requestBody["symbolFilePath"] = symbolFilePath;
        requestBody["versionNumber"] = versionNumber;
        
        return await postJSON(`http://${this.host}:${this.port}/export/zip/addFile`, requestBody);    
    }      
}