import {fetchJSON, postJSON} from "./BaseApi.mjs";

export default class MonitorApi {
    #host;
    #port;

    constructor(hostName, port) {
        this.#host = hostName;
        this.#port = port;
    }

    async getNewAddedList() {
        return await fetchJSON(`http://${this.#host}:${this.#port}/newAddedList`);
    }

    async getDeletedList() {
        return await fetchJSON(`http://${this.#host}:${this.#port}/deletedList`);
    }

    async getUpdatedFileList() {
        return await fetchJSON(`http://${this.#host}:${this.#port}/updatedFileList`);
    }
}