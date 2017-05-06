static uint64_t generate_cas_unique(void)
{
	return (int64_t)rand();
}

static void handle_client(int client_sockfd)
{
	printf("Client %d connected.\n", client_sockfd);
	char buffer[CLIENT_BUFFER_SIZE] = {0};

	/*
	* read each command from the client
	*/
	while(1) {
		memset(buffer, 0, sizeof buffer);
		read(client_sockfd, buffer, sizeof buffer);

		if (strncmp(buffer, "quit", 4) == 0) {
			close(client_sockfd);
			return;
		}

		/* get and gets */
		if (strncmp(buffer, "get", 3) == 0) {
			unsigned gets_flag = 0;
			if (strncmp(buffer, "gets ", 4) == 0)
				gets_flag = 1;
			/* Assert not any arbitrary command beginning with get */
			if (strncmp(buffer + strlen("get") + gets_flag, " ", 1) != 0) {
				ERROR;
				continue;
			}

			buffer[strcspn(buffer, "\r\n")] = '\0';
			char *key = strtok(buffer + 4, WHITESPACE);

			std::lock_guard<std::mutex> guard(map_mutex);
			while (key) {
				if ((*map).count(key) != 0) {
					cache_entry *entry = &(*map)[key];
					write_VALUE(client_sockfd, entry, gets_flag);
					write(client_sockfd, entry->data, entry->bytes + 2);
				}
				key = strtok(NULL, WHITESPACE);
			}
			END;
			continue;
		}

		if (strncmp(buffer, "cas ", 4) == 0) {
			ssize_t len;
			cache_entry *entry;
			char readbuffer[CLIENT_BUFFER_SIZE] = {0};
			char *key = strtok(buffer + 4, WHITESPACE);
			if (!key) {
				ERROR;
				continue;
			}

			char *flags = strtok(NULL, WHITESPACE);
			if (!flags) {
				ERROR;
				continue;
			}

			char *expiry = strtok(NULL, WHITESPACE);
			if (!expiry) {
				ERROR;
				continue;
			}

			char *bytes = strtok(NULL, WHITESPACE);
			if (!bytes) {
				ERROR;
				continue;
			}

			char *cas_unique = strtok(NULL, WHITESPACE);
			if (!cas_unique) {
				ERROR;
				continue;
			}

			len = 0;
			while (len < atoi(bytes)) {
				len += read(client_sockfd, readbuffer + len, sizeof readbuffer - len);
			}

			std::lock_guard<std::mutex> guard(map_mutex);
			if ((*map).count(key) == 0) {
				NOT_FOUND;
				continue;
			}
			entry = &(*map)[key];

			/* cas stores data only if no one else has updated the data since client read it last */
			if (entry->cas_unique != atoi(cas_unique)) {
				EXISTS;
				continue;
			}

			/* 2 is the size of \r\n */
			len -= 2;
			if (len < 1 || len > atoi(bytes)) {
				ERROR;
				CLIENT_ERROR("bad data chunk");
				continue;
			}

			/* delete the key */
			map->erase(key);
			memory_counter -= sizeof(cache_entry);
			free(entry->data);
			memory_counter -= entry->bytes;
			remove_from_list(entry);

			/* create new entry */
			entry = (cache_entry*) malloc(sizeof(cache_entry));
			memory_counter += sizeof(cache_entry);
			printf("%s: %u\n", "counter", memory_counter);
			entry->key = key;
			entry->flags = atoi(flags);
			entry->expiry = atoi(expiry);
			entry->bytes = atoi(bytes);
			entry->cas_unique = generate_cas_unique();

			entry->data = (char*)malloc(entry->bytes + 2);
			memory_counter += entry->bytes;
			memcpy(entry->data, readbuffer, entry->bytes + 2);

			/* CHECK FOR THRESHOLD BREACH */
			printf("%s: %u\n", "counter", memory_counter);
			if (memory_counter > MEMORY_THRESHOLD) {
				int ret = run_replacement(entry->bytes);
				if (ret) {
					free(entry);
					ERROR;
					SERVER_ERROR("Out of memory");
					continue;
				}
			}
			add_to_list(entry);

			(*map)[entry->key] = *entry;
			STORED;
			continue;

		}

		if (strncmp(buffer, "add ", 4) == 0) {
		

		/* default case */
		ERROR;

		}

		if (strncmp(buffer, "replace ", 8) == 0) { //8?
		

		/* default case */
		ERROR;
		}

		if (strncmp(buffer, "append ", 7) == 0) {
		
		//No flags or exp time
		
		ssize_t len;
		char *key = strtok(buffer + 4, WHITESPACE);
		if (!key) {
			ERROR;
			continue;
		}

		char *bytes = strtok(NULL, WHITESPACE);
		if (!bytes) {
			ERROR;
			continue;
		}

		while (key) {
			if ((*map).count(key) != 0) {
				cache_entry *entry = &(*map)[key];
				///////////
			}
			key = strtok(NULL, WHITESPACE);
		}


		/* default case */
		ERROR;
		}

		if (strncmp(buffer, "prepend ", 8) == 0) { 
		
		//No flags or exp time

		ssize_t len;
		char *key = strtok(buffer + 4, WHITESPACE);
		if (!key) {
			ERROR;
			continue;
		}

		char *bytes = strtok(NULL, WHITESPACE);
		if (!bytes) {
			ERROR;
			continue;
		}		

		/* default case */
		ERROR;
		}

		if (strncmp(buffer, "gets ", 5) == 0) { 


		END;
		continue;
		}

		if (strncmp(buffer, "delete ", 7) == 0) {


		END;
		continue;
		}

		if (strncmp(buffer, "incr ", 5) == 0) { 


		END;
		continue;
		}
		
		if (strncmp(buffer, "decr ", 5) == 0) { 


		END;
		continue;
		}

		if (strncmp(buffer, "stats ", 6) == 0) { 

		}
				

		if (strncmp(buffer, "set ", 4) == 0) {
			ssize_t len;
			char *key = strtok(buffer + 4, WHITESPACE);
			if (!key) {
				ERROR;
				continue;
			}

			char *flags = strtok(NULL, WHITESPACE);
			if (!flags) {
				ERROR;
				continue;
			}

			char *expiry = strtok(NULL, WHITESPACE);
			if (!expiry) {
				ERROR;
				continue;
			}

			char *bytes = strtok(NULL, WHITESPACE);
			if (!bytes) {
				ERROR;
				continue;
			}

			cache_entry *entry = (cache_entry*) malloc(sizeof(cache_entry));
			memory_counter += sizeof(cache_entry);
			printf("sizeof(cache_entry): %lu\n", sizeof(cache_entry));
			printf("%s: %u\n", "counter", memory_counter);
			entry->key = key;
			entry->flags = atoi(flags);
			entry->expiry = atoi(expiry);
			entry->bytes = atoi(bytes);
			entry->cas_unique = generate_cas_unique();

			/* Read actual data and add to map*/
			memset(buffer, 0, sizeof buffer);
			len = 0;
			while (len < entry->bytes) {
				len += read(client_sockfd, buffer + len, sizeof buffer - len);
			}

			/* 2 is the size of \r\n */
			len -= 2;
			if (len < 1) {
				free(entry);
				ERROR;
				CLIENT_ERROR("bad data chunk");
				continue;
			}
			if (len > entry->bytes) {
				free(entry);
				ERROR;
				continue;
			}
			/* reassign so that bytes is not greater than len */
			entry->bytes = (uint32_t)len;
			entry->data = (char*)malloc(entry->bytes + 2);
			memory_counter += entry->bytes;
			memcpy(entry->data, buffer, entry->bytes + 2);

			std::lock_guard<std::mutex> guard(map_mutex);
			/* CHECK FOR THRESHOLD BREACH */
			printf("%s: %u\n", "counter", memory_counter);
			if (memory_counter > MEMORY_THRESHOLD) {
				int ret = run_replacement(entry->bytes);
				if (ret) {
					free(entry);
					ERROR;
					SERVER_ERROR("Out of memory");
					continue;
				}
			}
			add_to_list(entry);

			(*map)[entry->key] = *entry;
			STORED;
			continue;
		}

		/* default case */
		ERROR;
	}
}

