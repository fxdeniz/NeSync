import {fetchJSON, postJSON, BaseApi} from "./BaseApi.mjs";

export default class ZipExportApi extends BaseApi {

    constructor(hostName, port) {
        super(hostName, port);
    }

    async setExportZipFilePath(filePath) {
        let requestBody = {"filePath": null};
        requestBody["filePath"] = filePath;
        
        return await postJSON(`http://${this.host}:${this.port}/postSetZipFilePath`, requestBody);
    }

    async getZipFilePath() {
        return await fetchJSON(`http://${this.host}:${this.port}/getZipFilePath`);    
    }

    async setRootSymbolFolderPath(rootPath) {
        let requestBody = {"rootSymbolFolderPath": null};
        requestBody["rootSymbolFolderPath"] = rootPath;
        
        return await postJSON(`http://${this.host}:${this.port}/postSetRootSymbolFolderPath`, requestBody);    
    }

    async createZipArchive(filePath) {
        let requestBody = {"filePath": null};
        requestBody["filePath"] = filePath;
        
        return await postJSON(`http://${this.host}:${this.port}/postCreateZipArchive`, requestBody);    
    }

    async addFilesJson() {
        let requestBody = {};
        return await postJSON(`http://${this.host}:${this.port}/postAddFileJson`, requestBody);    
    }

    async addFoldersJson() {
        let requestBody = {};
        return await postJSON(`http://${this.host}:${this.port}/postAddFolderJson`, requestBody);    
    }

    async addFileToZip(symbolFilePath, versionNumber) {
        let requestBody = {"symbolFilePath": null, "versionNumber": null};
        requestBody["symbolFilePath"] = symbolFilePath;
        requestBody["versionNumber"] = versionNumber;
        
        return await postJSON(`http://${this.host}:${this.port}/postAddFileToZip`, requestBody);    
    }      
}