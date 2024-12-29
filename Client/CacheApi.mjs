export default class CacheApi {
    #cache;

    constructor() {
        this.#cache = {};
    }

    get(key) {
        return this.#cache[`${key}`] ? this.#cache[`${key}`] : null;
    }

    set(key, value) {
        this.#cache[`${key}`] = value;
    }
}