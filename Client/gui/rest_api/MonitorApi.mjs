import {fetchJSON, BaseApi} from "./BaseApi.mjs";

export default class MonitorApi extends BaseApi {

    constructor(hostName, port) {
        super(hostName, port);
    }

    async getHeartBeat() {
        return await fetchJSON(`http://${this.host}:${this.port}/`);
    }

    async getNewAddedList() {
        return await fetchJSON(`http://${this.host}:${this.port}/monitor/new`);
    }

    async getDeletedList() {
        return await fetchJSON(`http://${this.host}:${this.port}/monitor/deleted`);
    }

    async getUpdatedFileList() {
        return await fetchJSON(`http://${this.host}:${this.port}/monitor/updated`);
    }
}