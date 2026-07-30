<script lang="ts">
	let {
		title,
		description,
		selected = false,
		onclick
	} = $props<{
		title: string;
		description: string;
		selected?: boolean;
		onclick?: () => void;
	}>();
</script>

<button class:selected {onclick} disabled={!onclick}>
	<div class="title">{title}</div>
	<div class="description">{description}</div>
</button>

<style>
	button {
		width: 100%;
		text-align: left;
		padding: 1rem;
		margin-bottom: 0.75rem;
		border: 1px solid transparent;
		border-radius: 16px;
		background: var(--surface);
		color: var(--text);
		cursor: pointer;
		transition:
			transform 120ms ease,
			border-color 200ms ease,
			background-color 200ms ease;
	}

	button:not(:disabled):hover {
		transform: translateY(-1px);
	}

	button:disabled {
		cursor: default;
	}

	button.selected {
		border-color: var(--accent);
	}

	.title {
		font-weight: 600;
		margin-bottom: 0.25rem;
	}

	.description {
		color: var(--text-secondary);
		font-size: 0.9rem;
	}
</style>
