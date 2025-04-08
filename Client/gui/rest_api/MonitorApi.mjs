import {fetchJSON, BaseApi} from "./BaseApi.mjs";

export default class MonitorApi extends BaseApi {

    constructor(hostName, port) {
        super(hostName, port);
    }

    async heartbeat() {
        return await fetchJSON(`http://${this.host}:${this.port}/`);
    }

    async newAddedList() {
        return await fetchJSON(`http://${this.host}:${this.port}/monitor/new`);
    }

    async deletedList() {
        return await fetchJSON(`http://${this.host}:${this.port}/monitor/deleted`);
    }

    async updatedFileList() {
        return await fetchJSON(`http://${this.host}:${this.port}/monitor/updated`);
    }
}