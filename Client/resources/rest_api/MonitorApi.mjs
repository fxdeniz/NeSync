import {fetchJSON, BaseApi} from "./BaseApi.mjs";

export default class MonitorApi extends BaseApi {

    constructor(hostName, port) {
        super(hostName, port);
    }

    async getNewAddedList() {
        return await fetchJSON(`http://${this.host}:${this.port}/newAddedList`);
    }

    async getDeletedList() {
        return await fetchJSON(`http://${this.host}:${this.port}/deletedList`);
    }

    async getUpdatedFileList() {
        return await fetchJSON(`http://${this.host}:${this.port}/updatedFileList`);
    }
}