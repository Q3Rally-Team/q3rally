#include "cg_local.h"

cg_ladder_t cg_ladder;

static void CG_LadderDropOldest(void) {
	if (cg_ladder.numEntries <= 0) {
		return;
	}

	if (cg_ladder.numEntries < MAX_LADDER_ENTRIES) {
		return;
	}

	memmove(&cg_ladder.entries[0],
			&cg_ladder.entries[1],
			(MAX_LADDER_ENTRIES - 1) * sizeof(cg_ladder_entry_t));
	cg_ladder.numEntries = MAX_LADDER_ENTRIES - 1;
}

static void CG_LadderUpdateTimestamp(void) {
	cg_ladder.lastUpdateTime = cg.time;
}

void CG_LadderClear(void) {
	Com_Memset(&cg_ladder, 0, sizeof(cg_ladder));
	cg_ladder.status = LADDER_STATUS_EMPTY;
}

void CG_LadderInit(void) {
	CG_LadderClear();
}

void CG_LadderBeginLoad(void) {
	CG_LadderClear();
	cg_ladder.status = LADDER_STATUS_LOADING;
	cg_ladder.dirty = qtrue;
	CG_LadderUpdateTimestamp();
}

void CG_LadderMarkReady(void) {
	if (cg_ladder.status == LADDER_STATUS_ERROR) {
		return;
	}

	if (cg_ladder.numEntries > 0) {
		cg_ladder.status = LADDER_STATUS_READY;
	} else {
		cg_ladder.status = LADDER_STATUS_EMPTY;
	}

	CG_LadderUpdateTimestamp();
}

void CG_LadderSetError(const char *message) {
	if (!message) {
		message = "";
	}

	cg_ladder.status = LADDER_STATUS_ERROR;
	Q_strncpyz(cg_ladder.errorMessage, message, sizeof(cg_ladder.errorMessage));
	cg_ladder.dirty = qtrue;
	CG_LadderUpdateTimestamp();
}

static void CG_LadderPushEntryInternal(cg_ladder_source_t source,
									   int sequence,
									   const char *identifier,
									   const char *payload,
									   int serverTime) {
	cg_ladder_entry_t *entry;

	if (cg_ladder.status == LADDER_STATUS_ERROR) {
		return;
	}

	if (!identifier) {
		identifier = "";
	}

	if (!payload) {
		payload = "";
	}

	if (cg_ladder.numEntries >= MAX_LADDER_ENTRIES) {
		CG_LadderDropOldest();
	}

	entry = &cg_ladder.entries[cg_ladder.numEntries++];
	entry->source = source;
	entry->sequence = sequence;
	entry->serverTime = serverTime;
	Q_strncpyz(entry->identifier, identifier, sizeof(entry->identifier));
	Q_strncpyz(entry->payload, payload, sizeof(entry->payload));

	if (cg_ladder.status == LADDER_STATUS_EMPTY) {
		cg_ladder.status = LADDER_STATUS_READY;
	}

	cg_ladder.dirty = qtrue;
	CG_LadderUpdateTimestamp();
}

void CG_LadderPushEntry(cg_ladder_source_t source,
						int sequence,
						const char *identifier,
						const char *payload,
						int serverTime) {
	CG_LadderPushEntryInternal(source, sequence, identifier, payload, serverTime);
}

void CG_LadderHandleServerCommand(int sequence,
								  const char *identifier,
								  const char *payload,
								  int serverTime) {
	CG_LadderPushEntryInternal(LADDER_SOURCE_SERVER,
							   sequence,
							   identifier,
							   payload,
							   serverTime);
}

void CG_LadderHandleWebResponse(const char *identifier, const char *payload) {
	const char *cursor;
	char line[MAX_STRING_CHARS];
	int lineSequence = 0;

	if (!payload) {
		payload = "";
	}

	CG_LadderBeginLoad();

	cursor = payload;
	while (*cursor) {
		int len = 0;

		while (*cursor && *cursor != '\n' && len < MAX_STRING_CHARS - 1) {
			line[len++] = *cursor++;
		}
		line[len] = '\0';

		if (*cursor == '\n') {
			cursor++;
		}

		if (len > 0 || lineSequence == 0) {
			CG_LadderPushEntryInternal(LADDER_SOURCE_WEBSERVICE,
									   lineSequence,
									   identifier,
									   line,
									   cg.time);
			lineSequence++;
		}
	}

	if (cg_ladder.status != LADDER_STATUS_ERROR) {
		CG_LadderMarkReady();
	}
}

const cg_ladder_t *CG_LadderState(void) {
	return &cg_ladder;
}

qboolean CG_LadderIsLoading(void) {
	return (cg_ladder.status == LADDER_STATUS_LOADING);
}

qboolean CG_LadderHasError(void) {
	return (cg_ladder.status == LADDER_STATUS_ERROR);
}
